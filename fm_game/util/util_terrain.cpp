
#include "helper.h"
#include "global.h"
#include "util_terrain.h"
#include "../tools/util_func.h"

bool get_next_height(ITerrain* pTerrain, IVisBase* pRole, float dx, float dz, float step, float& floorY, int& floorIndex)
{
  if(pTerrain == NULL || pRole == NULL)
    return false;

  float cur_y = pRole->GetPosition().y;

  // 可行走层数
  int floor_num = pTerrain->GetFloorCount(dx, dz);

  float fNearFloorY = 1e8;
  int nNearFloor = 0;
  for (int i = (floor_num - 1); i >= 0; --i)
  {
    bool can_move = pTerrain->GetFloorCanMove(dx, dz, i);
    bool can_stand = pTerrain->GetFloorCanStand(dx, dz, i);
    if (can_move && can_stand)
    {
      float floor_y = pTerrain->GetFloorHeight(dx, dz, i);

      // 是否合适的高度
      if ((floor_y <= (cur_y + step)) && (floor_y >= (cur_y - step)))
      {	
        if (fabs(floor_y - cur_y) < fabs(fNearFloorY - cur_y))
        {
          fNearFloorY = floor_y;
          nNearFloor = i;
        }
        continue;
      }
    }
  }

  if (fNearFloorY < 1e7)
  {
    floorY = fNearFloorY;
    floorIndex = nNearFloor;
    return true;
  }
  return false;
}

bool check_terrain_walk_condition_for_joystick(ITerrain* pTerrain, IVisBase* pRole, float fDestX, float fDestZ, float& fFinalMoveAngle, 
                                               float& fFinalDistance, int& nFinalFloor, float& fFinalX, float& fFinalY, float& fFinalZ)
{
  //无法到达时尝试向其他方向移动，避免原地踏步
  if (get_next_floor(pTerrain, pRole, fDestX, fDestZ, nFinalFloor, fFinalX, fFinalY, fFinalZ))
  {
    fFinalDistance = util_dot_distance(pRole->GetPositionX(), pRole->GetPositionZ(), fFinalX, fFinalZ);
    return true;
  }
  else
  {
    //减小移动距离
    fFinalDistance = fFinalDistance * 0.5f;

    //确定移动方向属于第几区间
    int nQuadIndex = ((int)(fFinalMoveAngle / PI * 2.0f))%4;
    const float DIRECT_FOR_QUAD[4][2] = 
    {
      {  1.0f,  1.0f }, //第一区间尝试右上移动
      {  1.0f, -1.0f }, //第二区间尝试右下移动
      { -1.0f, -1.0f }, //第三区间尝试左下移动
      { -1.0f,  1.0f }  //第四区间尝试左上移动   
    };

    float fToRight = fFinalDistance * DIRECT_FOR_QUAD[nQuadIndex][0];
    float fToTop = fFinalDistance * DIRECT_FOR_QUAD[nQuadIndex][1];

    //尝试X(左右)方向上的移动
    if (get_next_floor(pTerrain, pRole, pRole->GetPositionX() + fToRight, pRole->GetPositionZ(), nFinalFloor, fFinalX, fFinalY, fFinalZ))
    {
      fFinalMoveAngle = ((fToRight > 0) ? (PI * 0.5f) : (PI * 1.5f));
      fFinalDistance = util_dot_distance(pRole->GetPositionX(), pRole->GetPositionZ(), fFinalX, fFinalZ);
      return true;
    }
    //尝试Z(上下)方向上的移动
    else if (get_next_floor(pTerrain, pRole, pRole->GetPositionX(), pRole->GetPositionZ() + fToTop, nFinalFloor, fFinalX, fFinalY, fFinalZ))
    {
      fFinalMoveAngle = ((fToTop > 0) ? 0.f : PI);
      fFinalDistance = util_dot_distance(pRole->GetPositionX(), pRole->GetPositionZ(), fFinalX, fFinalZ);
      return true;
    }
  }
  return false;
}


// 考虑地形障碍，获得下一个位置的可行走层和高度
bool get_next_floor(ITerrain* pTerrain, IVisBase* pRole, float fDestX, float fDestZ,
                    int& iFloorIndex, float& fNewX, float& fNewY, float& fNewZ, float fAllowHeightDiff/* = 1.2f*/)
{
  if (pTerrain == NULL || pRole == NULL)
  {
    return false;
  }

  //碰撞半径
  float fCollideRadius = CHelper::QueryCustomFloat(pRole, "collide_radius");

  // 起点位置
  FmVec3 StartVec = pRole->GetPosition();
  float fStartX = StartVec.x;
  float fStartY = StartVec.y;
  float fStartZ = StartVec.z;

  // 获得所有经过的碰撞格坐标,考虑对象的碰撞半径
  FmVec2 vDeltaDir(fDestX - fStartX, fDestZ - fStartZ);
  FmVec2 vNormalDir = vDeltaDir.normalized();

  FmVec2 vDestPos(fDestX, fDestZ);
  FmVec2 vForwardPos = vDestPos + vNormalDir * fCollideRadius; 

  CVarList gridTable;
  pTerrain->GetLineTraverse(CVarList() << fStartX << fStartZ << vForwardPos.x << vForwardPos.y, gridTable);

  int iGridNum = (int)(gridTable.GetCount() >> 1);
  if (iGridNum < 1)
  {
    iFloorIndex = CHelper::QueryCustomInt(pRole, "floor_index");
    fNewX = fDestX;
    fNewY = fStartY;
    fNewZ = fDestZ;
    return false;
  }
  else if (iGridNum == 1)
  {
    // 就在当前的碰撞格中
    iFloorIndex = CHelper::QueryCustomInt(pRole, "floor_index");
    fNewX = fDestX;
    fNewY = fStartY;
    fNewZ = fDestZ;
    return true;
  }

  // 可用的层坐标
  int avail_floor = CHelper::QueryCustomInt(pRole, "floor_index");
  float avail_x = fStartX;
  float avail_y = fStartY;
  float avail_z = fStartZ;

  // 判断每一个格子的碰撞
  for (int n = 1; n < iGridNum; ++n)
  {
    float grid_x = gridTable.FloatVal(n<<1);
    float grid_z = gridTable.FloatVal((n<<1) + 1);

    bool can_move = false;

    // 可行走层数
    int fFloorCount = pTerrain->GetFloorCount(grid_x, grid_z);

    int higher_floor_index = iGridNum;
    float higher_floor_height = 10000;
    bool higher_canreach = false;

    int lower_floor_index = -1;
    float lower_floor_height = -10000;
    bool lower_canreach = false;

    //遍历层
    for (int i = fFloorCount - 1; i >= 0; --i)
    {
      bool bFloorExists = pTerrain->GetFloorExists(grid_x, grid_z, i);
      if (bFloorExists)
      {
        float fFloorY = pTerrain->GetFloorHeight(grid_x, grid_z, i);
        bool bCanMove = pTerrain->GetFloorCanMove(grid_x, grid_z, i);
        bool bCanStand = pTerrain->GetFloorCanStand(grid_x, grid_z, i);

        // 在目标高度之上,并小于当前高度则取之
        if ((fStartY <= fFloorY) && (fFloorY < higher_floor_height))
        {
          higher_floor_height = fFloorY;
          higher_floor_index = i;
          higher_canreach = bCanMove & bCanStand;
        }

        // 在目标高度之下，但比我当前的高则需之
        if ((fStartY >= fFloorY) && (fFloorY > lower_floor_height))
        {
          lower_floor_height = fFloorY;
          lower_floor_index = i;
          lower_canreach = bCanMove & bCanStand;
        }

        if (bCanMove && bCanStand)
        {	
          // 是否合适的高度
          if ((fFloorY <= (fStartY + fAllowHeightDiff)) && (fFloorY >= (fStartY - fAllowHeightDiff)))
          {
            can_move = true;
            fStartY = fFloorY;

            // 记录可行走的格子
            avail_floor = i;
            avail_x = grid_x;
            avail_y = fFloorY;
            avail_z = grid_z;
            break;
          }
        }
        else
        {
          // 是否合适的高度
          if ((fFloorY <= (fStartY + fAllowHeightDiff)) && (fFloorY >= (fStartY - fAllowHeightDiff)))
          {
            break;
          }
        }
      }
    }

    if (!can_move)
    {
      // 碰到不可行走的格子
      if (n == 1 || n == 2)
      {
        //szResult = no_result;
        iFloorIndex = CHelper::QueryCustomInt(pRole, "floor_index");
        fNewX = fStartX;
        fNewY = fStartY;
        fNewZ = fStartZ;

        return false;
      }

      // 移动到前一个格子,并回退碰撞半径
      FmVec2 vNewDeltaDir(avail_x - fStartX, avail_z - fStartZ);
      float fDeltaDist = vNewDeltaDir.length();
      if (fDeltaDist < fCollideRadius)
      {                
        iFloorIndex = CHelper::QueryCustomInt(pRole, "floor_index");
        fNewX = fStartX;
        fNewY = fStartY;
        fNewZ = fStartZ;
        return false;
      }
      else
      {
        iFloorIndex = avail_floor;
        FmVec2 vNewNormalDir = vNewDeltaDir.normalized();
        FmVec2 vBackwardPos = FmVec2(avail_x, avail_z) - vNewNormalDir * fCollideRadius;
        fNewX = vBackwardPos.x;
        fNewZ = vBackwardPos.y;
        fNewY = get_cur_floor_height(pTerrain, vBackwardPos.x, avail_y, vBackwardPos.y);
        return true;
      }
    }
  }

  // 移动到目标点
  iFloorIndex = avail_floor;
  fNewX = fDestX;
  fNewY = avail_y;
  fNewZ = fDestZ;
  return true; 
}

float get_cur_floor_height(ITerrain* pTerrain, float x, float y, float z, int* outfloor /*= NULL*/)
{
  if(pTerrain == NULL)
    return 0.0f;
  // 遍历所有层比较距离
  float dis_min = 100000.f;
  // 目标层高
  float real_y = y;

  // 遍历层
  int count = pTerrain->GetFloorCount(x, z);

  for (int i = 0; i < count; ++i)
  {
    // 判断层是否存在
    if (!pTerrain->GetFloorExists(x, z, i) ||
      !pTerrain->GetFloorCanMove(x, z, i) ||
      !pTerrain->GetFloorCanStand(x, z, i))
    {
      continue;
    }
    // 当前层高度
    float floor_y = pTerrain->GetFloorHeight(x, z, i);

    // 判断当前层和高度的差距
    float dis = fabs(y - floor_y);
    if (dis < dis_min)
    {
      dis_min = dis;
      real_y = floor_y;

      if (outfloor)
      {
        *outfloor = i;
      }
    }
  }
  return real_y;
}

