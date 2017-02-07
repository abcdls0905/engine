
package com.android.fmdemo;

import android.content.Context;
import android.text.Editable;
import android.text.TextWatcher;
import android.util.Log;
import android.view.KeyEvent;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputMethodManager;
import android.widget.TextView;

/**
 * Created with IntelliJ IDEA.
 * User: zjs
 * Date: 13-6-25
 * Time: 下午3:52
 * To change this template use File | Settings | File Templates.
 */
public class TextInputWraper  implements TextWatcher, TextView.OnEditorActionListener {
    // ===========================================================
    // Constants
    // ===========================================================

    private static final String TAG = TextInputWraper.class.getSimpleName();

    // ===========================================================
    // Fields
    // ===========================================================

    private final FMDEMOView mGLSurfaceView;
    private String mText;
    private String mOriginText;

    // ===========================================================
    // Constructors
    // ===========================================================

    public TextInputWraper(final FMDEMOView pCocos2dxGLSurfaceView) {
        mGLSurfaceView = pCocos2dxGLSurfaceView;
    }

    // ===========================================================
    // Getter & Setter
    // ===========================================================

    private boolean isFullScreenEdit() {
        final TextView textField = mGLSurfaceView.getEditText();
        final InputMethodManager imm = (InputMethodManager) textField.getContext().getSystemService(Context.INPUT_METHOD_SERVICE);
        return imm.isFullscreenMode();
    }

    public void setOriginText(final String pOriginText) {
        this.mOriginText = pOriginText;
    }

    // ===========================================================
    // Methods for/from SuperClass/Interfaces
    // ===========================================================

    @Override
    public void afterTextChanged(final Editable s) {
        if (this.isFullScreenEdit()) {
            return;
        }


        //if (BuildConfig.DEBUG) {
        //Log.d(TAG, "afterTextChanged: " + s);
        //}
        int nModified = s.length() - this.mText.length();
        if (nModified > 0) {
            final String insertText = s.subSequence(this.mText.length(), s.length()).toString();
            FMDEMOLib.inserttext(insertText);
            Log.d("fm", String.format("afterTextChanged %s", insertText));
          //  this.mGLSurfaceView.insertText(insertText);
			/*
			if (BuildConfig.DEBUG) {
				Log.d(TAG, "insertText(" + insertText + ")");
			}
			*/
        } else {
            for (; nModified < 0; ++nModified) {
             //   this.mGLSurfaceView.deleteBackward();
				/*
				if (BuildConfig.DEBUG) {
					Log.d(TAG, "deleteBackward");
				}
				*/
                FMDEMOLib.backspace();
                Log.d("fm", "deleteBackward");
            }
        }
        this.mText = s.toString();
    }

    @Override
    public void beforeTextChanged(final CharSequence pCharSequence, final int start, final int count, final int after) {
		/*
		if (BuildConfig.DEBUG) {
			Log.d(TAG, "beforeTextChanged(" + pCharSequence + ")start: " + start + ",count: " + count + ",after: " + after);
		}
		*/
        this.mText = pCharSequence.toString();
    }

    @Override
    public void onTextChanged(final CharSequence pCharSequence, final int start, final int before, final int count) {

    }

    @Override
    public boolean onEditorAction(final TextView pTextView, final int pActionID, final KeyEvent pKeyEvent) {
        if (this.mGLSurfaceView.getEditText() == pTextView && this.isFullScreenEdit()) {
            // user press the action button, delete all old text and insert new text
            /*
            for (int i = this.mOriginText.length(); i > 0; i--) {

                Log.d(TAG, "deleteBackward");

            }  */

            for (int i = this.mOriginText.length(); i > 0; i--) {
                FMDEMOLib.backspace();
               // this.mCocos2dxGLSurfaceView.deleteBackward();
				/*
				if (BuildConfig.DEBUG) {
					Log.d(TAG, "deleteBackward");
				}
				*/
            }

            String text = pTextView.getText().toString();

			/* If user input nothing, translate "\n" to engine. */
            if (text.compareTo("") == 0) {
                text = "\n";
            }

            if ('\n' != text.charAt(text.length() - 1)) {
                text += '\n';
            }

            final String insertText = text;
            FMDEMOLib.inserttext(insertText);
            Log.d("fm", String.format("insert text %s", insertText));
            //暂时注掉
           // this.mGLSurfaceView.insertText(insertText);
			/*
			if (BuildConfig.DEBUG) {
				Log.d(TAG, "insertText(" + insertText + ")");
			}
			*/
        }

        if (pActionID == EditorInfo.IME_ACTION_DONE) {
            this.mGLSurfaceView.requestFocus();
        }
        return false;
    }

    // ===========================================================
    // Methods
    // ===========================================================

    // ===========================================================
    // Inner and Anonymous Classes
    // ===========================================================
}
