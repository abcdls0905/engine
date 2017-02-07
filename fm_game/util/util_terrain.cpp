
#include "helper.h"
#include "global.h"
#include "util_terrain.h"
#include "../tools/util_func.h"

bool get_next_height(ITerrain* pTerrain, IVisBase* pRole, float dx, float dz, float step, float& floorY, int& floorIndex)
{
  if(pTerrain == NULL || pRole == NULL)
    return false;

  float cur_y = pRole->GetPosition().y;

  // �����߲���
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

      // �Ƿ���ʵĸ߶�
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
  //�޷�����ʱ���������������ƶ�������ԭ��̤��
  if (get_next_floor(pTerrain, pRole, fDestX, fDestZ, nFinalFloor, fFinalX, fFinalY, fFinalZ))
  {
    fFinalDistance = util_dot_distance(pRole->GetPositionX(), pRole->GetPositionZ(), fFinalX, fFinalZ);
    return true;
  }
  else
  {
    //��С�ƶ�����
    fFinalDistance = fFinalDistance * 0.5f;

    //ȷ���ƶ��������ڵڼ�����
    int nQuadIndex = ((int)(fFinalMoveAngle / PI * 2.0f))%4;
    const float DIRECT_FOR_QUAD[4][2] = 
    {
      {  1.0f,  1.0f }, //��һ���䳢�������ƶ�
      {  1.0f, -1.0f }, //�ڶ����䳢�������ƶ�
      { -1.0f, -1.0f }, //�������䳢�������ƶ�
      { -1.0f,  1.0f }  //�������䳢�������ƶ�   
    };

    float fToRight = fFinalDistance * DIRECT_FOR_QUAD[nQuadIndex][0];
    float fToTop = fFinalDistance * DIRECT_FOR_QUAD[nQuadIndex][1];

    //����X(����)�����ϵ��ƶ�
    if (get_next_floor(pTerrain, pRole, pRole->GetPositionX() + fToRight, pRole->GetPositionZ(), nFinalFloor, fFinalX, fFinalY, fFinalZ))
    {
      fFinalMoveAngle = ((fToRight > 0) ? (PI * 0.5f) : (PI * 1.5f));
      fFinalDistance = util_dot_distance(pRole->GetPositionX(), pRole->GetPositionZ(), fFinalX, fFinalZ);
      return true;
    }
    //����Z(����)�����ϵ��ƶ�
    else if (get_next_floor(pTerrain, pRole, pRole->GetPositionX(), pRole->GetPositionZ() + fToTop, nFinalFloor, fFinalX, fFinalY, fFinalZ))
    {
      fFinalMoveAngle = ((fToTop > 0) ? 0.f : PI);
      fFinalDistance = util_dot_distance(pRole->GetPositionX(), pRole->GetPositionZ(), fFinalX, fFinalZ);
      return true;
    }
  }
  return false;
}


// ���ǵ����ϰ��������һ��λ�õĿ����߲�͸߶�
bool get_next_floor(ITerrain* pTerrain, IVisBase* pRole, float fDestX, float fDestZ,
                    int& iFloorIndex, float& fNewX, float& fNewY, float& fNewZ, float fAllowHeightDiff/* = 1.2f*/)
{
  if (pTerrain == NULL || pRole == NULL)
  {
    return false;
  }

  //��ײ�뾶
  float fCollideRadius = CHelper::QueryCustomFloat(pRole, "collide_radius");

  // ���λ��
  FmVec3 StartVec = pRole->GetPosition();
  float fStartX = StartVec.x;
  float fStartY = StartVec.y;
  float fStartZ = StartVec.z;

  // ������о�������ײ������,���Ƕ������ײ�뾶
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
    // ���ڵ�ǰ����ײ����
    iFloorIndex = CHelper::QueryCustomInt(pRole, "floor_index");
    fNewX = fDestX;
    fNewY = fStartY;
    fNewZ = fDestZ;
    return true;
  }

  // ���õĲ�����
  int avail_floor = CHelper::QueryCustomInt(pRole, "floor_index");
  float avail_x = fStartX;
  float avail_y = fStartY;
  float avail_z = fStartZ;

  // �ж�ÿһ�����ӵ���ײ
  for (int n = 1; n < iGridNum; ++n)
  {
    float grid_x = gridTable.FloatVal(n<<1);
    float grid_z = gridTable.FloatVal((n<<1) + 1);

    bool can_move = false;

    // �����߲���
    int fFloorCount = pTerrain->GetFloorCount(grid_x, grid_z);

    int higher_floor_index = iGridNum;
    float higher_floor_height = 10000;
    bool higher_canreach = false;

    int lower_floor_index = -1;
    float lower_floor_height = -10000;
    bool lower_canreach = false;

    //������
    for (int i = fFloorCount - 1; i >= 0; --i)
    {
      bool bFloorExists = pTerrain->GetFloorExists(grid_x, grid_z, i);
      if (bFloorExists)
      {
        float fFloorY = pTerrain->GetFloorHeight(grid_x, grid_z, i);
        bool bCanMove = pTerrain->GetFloorCanMove(grid_x, grid_z, i);
        bool bCanStand = pTerrain->GetFloorCanStand(grid_x, grid_z, i);

        // ��Ŀ��߶�֮��,��С�ڵ�ǰ�߶���ȡ֮
        if ((fStartY <= fFloorY) && (fFloorY < higher_floor_height))
        {
          higher_floor_height = fFloorY;
          higher_floor_index = i;
          higher_canreach = bCanMove & bCanStand;
        }

        // ��Ŀ��߶�֮�£������ҵ�ǰ�ĸ�����֮
        if ((fStartY >= fFloorY) && (fFloorY > lower_floor_height))
        {
          lower_floor_height = fFloorY;
          lower_floor_index = i;
          lower_canreach = bCanMove & bCanStand;
        }

        if (bCanMove && bCanStand)
        {	
          // �Ƿ���ʵĸ߶�
          if ((fFloorY <= (fStartY + fAllowHeightDiff)) && (fFloorY >= (fStartY - fAllowHeightDiff)))
          {
            can_move = true;
            fStartY = fFloorY;

            // ��¼�����ߵĸ���
            avail_floor = i;
            avail_x = grid_x;
            avail_y = fFloorY;
            avail_z = grid_z;
            break;
          }
        }
        else
        {
          // �Ƿ���ʵĸ߶�
          if ((fFloorY <= (fStartY + fAllowHeightDiff)) && (fFloorY >= (fStartY - fAllowHeightDiff)))
          {
            break;
          }
        }
      }
    }

    if (!can_move)
    {
      // �����������ߵĸ���
      if (n == 1 || n == 2)
      {
        //szResult = no_result;
        iFloorIndex = CHelper::QueryCustomInt(pRole, "floor_index");
        fNewX = fStartX;
        fNewY = fStartY;
        fNewZ = fStartZ;

        return false;
      }

      // �ƶ���ǰһ������,��������ײ�뾶
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

  // �ƶ���Ŀ���
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
  // �������в�ȽϾ���
  float dis_min = 100000.f;
  // Ŀ����
  float real_y = y;

  // ������
  int count = pTerrain->GetFloorCount(x, z);

  for (int i = 0; i < count; ++i)
  {
    // �жϲ��Ƿ����
    if (!pTerrain->GetFloorExists(x, z, i) ||
      !pTerrain->GetFloorCanMove(x, z, i) ||
      !pTerrain->GetFloorCanStand(x, z, i))
    {
      continue;
    }
    // ��ǰ��߶�
    float floor_y = pTerrain->GetFloorHeight(x, z, i);

    // �жϵ�ǰ��͸߶ȵĲ��
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

