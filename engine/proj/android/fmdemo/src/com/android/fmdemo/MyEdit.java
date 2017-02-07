package com.android.fmdemo;

import android.content.Context;
import android.util.AttributeSet;
import android.view.KeyEvent;
import android.widget.EditText;

/**
 * Created with IntelliJ IDEA.
 * User: zlc
 * Date: 13-6-25
 * Time: 上午11:52
 * To change this template use File | Settings | File Templates.
 */
public class MyEdit  extends EditText {

    private FMDEMOView mGLSurfaceView;
    public MyEdit(final Context context) {
        super(context);
    }

    public void setCocos2dxGLSurfaceView(final FMDEMOView pCocos2dxGLSurfaceView) {
        this.mGLSurfaceView = pCocos2dxGLSurfaceView;
    }

    public MyEdit(final Context context, final AttributeSet attrs) {
        super(context, attrs);
    }

    public MyEdit(final Context context, final AttributeSet attrs, final int defStyle) {
        super(context, attrs, defStyle);
    }

    @Override
    public boolean onKeyDown(final int pKeyCode, final KeyEvent pKeyEvent) {
        super.onKeyDown(pKeyCode, pKeyEvent);

		/* Let GlSurfaceView get focus if back key is input. */
        if (pKeyCode == KeyEvent.KEYCODE_BACK) {
            mGLSurfaceView.requestFocus();
        }

        return true;
    }
}
