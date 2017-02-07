/*
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.fmdemo;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.ViewGroup;
import android.view.ViewTreeObserver;
import android.view.WindowManager;
import android.widget.FrameLayout;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.os.Bundle;
import android.view.KeyEvent;
import java.io.File;


public class FMDEMOActivity extends Activity {

    FMDEMOView mView;

    public static int N_ACTIVITY_ID=8888;

    @Override protected void onCreate(Bundle icicle) {
        super.onCreate(icicle);
        mView = new FMDEMOView(getApplication());
         FMDEMOLib.setasset(getAssets());
        // FrameLayout
        ViewGroup.LayoutParams framelayout_params =
                new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT,
                        ViewGroup.LayoutParams.MATCH_PARENT);
        FrameLayout framelayout = new FrameLayout(this);

        ViewGroup.LayoutParams edittext_layout_params =
                new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT,
                        ViewGroup.LayoutParams.WRAP_CONTENT);


        framelayout.setLayoutParams(framelayout_params);

        MyEdit edittext = new MyEdit(this);
        edittext.setLayoutParams(edittext_layout_params);

        // ...add to FrameLayout
        framelayout.addView(edittext);



        mView.SetEditText(edittext);

        // ...add to FrameLayout
        framelayout.addView(mView);
       // mView.HideImeKeyBoard();
	    setContentView(framelayout);


        final String TAG = "FMDEMOIActivity";
        mView.getViewTreeObserver().addOnGlobalLayoutListener(new ViewTreeObserver.OnGlobalLayoutListener() {
            @Override
            public void onGlobalLayout() {
                int heightDiff = mView.getRootView().getHeight() - mView.getHeight();
                if (heightDiff > 100) { // if more than 100 pixels, its probably a keyboard...
                     Log.d(TAG,"xxxxxxxxxxxxxx");
                }
            }
        });



    }
	
	
	

    @Override protected void onPause() {
        super.onPause();
     //   mView.onPause();
    }

    @Override protected void onResume() {
        super.onResume();
        mView.onResume();
    }
	
	
	
    @Override
    protected void onDestroy() {
        super.onDestroy();
        android.os.Process.killProcess(android.os.Process.myPid());
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        if (keyCode == KeyEvent.KEYCODE_BACK) {
            showCloseWindown();
            return true;
        }
        return super.onKeyDown(keyCode, event);
    }

    public void showCloseWindown() {
        AlertDialog.Builder builder = new AlertDialog.Builder(activity);
        builder.setMessage("确定要退出游戏吗？").setCancelable(false).setPositiveButton("是", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                finish();
            }
        }).setNegativeButton("否", new DialogInterface.OnClickListener() {

            @Override
            public void onClick(DialogInterface dialog, int which) {
                dialog.cancel();
            }
        });
        AlertDialog alertDialog = builder.create();
        alertDialog.show();
    }
}
