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

import java.io.File;


public class FMDEMOActivity extends Activity {

    FMDEMOView mView;

    public static int N_ACTIVITY_ID=8888;

    @Override protected void onCreate(Bundle icicle) {
        super.onCreate(icicle);
        FMDEMOLib.setasset(getAssets());
        mView = new FMDEMOView(getApplication());
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
}
