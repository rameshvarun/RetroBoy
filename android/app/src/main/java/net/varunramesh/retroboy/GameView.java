package net.varunramesh.retroboy;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.os.Handler;
import android.os.Message;
import android.util.AttributeSet;
import android.util.DisplayMetrics;
import android.util.TypedValue;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

/**
 * Created by varunramesh on 10/7/15.
 */
public class GameView extends SurfaceView implements SurfaceHolder.Callback {

    private final GameThread thread;

    public GameView(Context context, AttributeSet attrs) {
        super(context, attrs);

        // Register our interest in hearing about changes to our surface
        SurfaceHolder holder = getHolder();
        holder.addCallback(this);

        // Create thread only; it's started in surfaceCreated()
        thread = new GameThread(holder, context);

        setFocusable(true); // make sure we get key events
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        thread.setRunning(true);
        thread.start();
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        // we have to tell thread to shut down & wait for it to finish, or else
        // it might touch the Surface after we return and explode
        boolean retry = true;
        thread.setRunning(false);
        while (retry) {
            try {
                thread.join();
                retry = false;
            } catch (InterruptedException e) {
            }
        }
    }

    public void setDevice(Device device) {
        thread.setDevice(device);
    }

    static class GameThread extends Thread {
        private boolean running;
        private final SurfaceHolder surfaceHolder;

        private Device device;
        private long lastTime = System.currentTimeMillis();


        private Bitmap dpad;
        private final DisplayMetrics displayMetrics;

        public void setRunning(boolean running) {
            this.running = running;
        }

        public void setDevice(Device device) {
            this.device = device;
        }

        public GameThread(SurfaceHolder holder, Context context) {
            surfaceHolder = holder;

            displayMetrics = context.getResources().getDisplayMetrics();
            try {
                dpad = BitmapFactory.decodeStream(context.getAssets().open("dpad_light.png"));
            } catch (Exception e) {
                e.printStackTrace();
            }
        }

        @Override
        public void run() {
            while (running) {
                Canvas c = null;
                try {
                    c = surfaceHolder.lockCanvas(null);
                    synchronized (surfaceHolder) {
                        // Draw if a device is available.
                        if (c != null) {
                            float aspect = 160.0f/144.0f;

                            boolean landscape = c.getWidth() > c.getHeight();

                            float scale = 1.0f;
                            if (c.getHeight() * aspect > c.getWidth()) {
                                // We are constrained by the width.
                                scale = ((float)c.getWidth()) / 160.0f;
                            } else {
                                // We are constrained by the height.
                                scale = ((float)c.getHeight()) / 144.0f;
                            }

                            c.drawColor(Color.BLACK);
                            if (device != null) {
                                device.draw();

                                float dt = (System.currentTimeMillis() - lastTime)/1000.0f;
                                device.update(dt);
                                lastTime = System.currentTimeMillis();

                                c.save();

                                // Center screen in landscape mode.
                                if (landscape)
                                    c.translate(c.getWidth() / 2.0f - 80*scale, 0);

                                c.scale(scale, scale);

                                c.drawBitmap(device.getDisplay(), 0, 0, null);
                                c.restore();

                                c.save();
                                float px = TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, 120, displayMetrics);
                                float dpadScale = (px / dpad.getWidth());

                                c.translate(40.0f, c.getHeight() * 0.75f - (dpad.getHeight()/2)*dpadScale);
                                c.scale(dpadScale, dpadScale);

                                c.drawBitmap(dpad, 0, 0, null);
                                c.restore();
                            }
                        }
                    }
                } finally {
                    // do this in a finally so that if an exception is thrown
                    // during the above, we don't leave the Surface in an
                    // inconsistent state
                    if (c != null) {
                        surfaceHolder.unlockCanvasAndPost(c);
                    }
                }
            }
        }
    }
}
