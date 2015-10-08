package net.varunramesh.retroboy;

import android.graphics.Bitmap;
import android.graphics.Color;
import android.util.Log;

import org.luaj.vm2.Globals;
import org.luaj.vm2.LuaDouble;
import org.luaj.vm2.LuaNil;
import org.luaj.vm2.LuaTable;
import org.luaj.vm2.LuaValue;
import org.luaj.vm2.lib.OneArgFunction;
import org.luaj.vm2.lib.ThreeArgFunction;
import org.luaj.vm2.lib.jse.JsePlatform;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.util.zip.ZipFile;
import java.util.zip.ZipInputStream;

/**
 * Created by varunramesh on 10/7/15.
 */
public class Device {
    private final Bitmap display;
    private final Globals vm;
    private final ZipFile filesystem;

    private final int[] palette = {
        Color.parseColor("#0f380f"), Color.parseColor("#306230"), Color.parseColor("#8bac0f"),
            Color.parseColor("#9bbc0f") };

    public Device(File zipfile) throws IOException {
        display = Bitmap.createBitmap(160, 144, Bitmap.Config.ARGB_8888);
        vm = JsePlatform.standardGlobals();
        filesystem = new ZipFile(zipfile);

        final LuaTable retroboy = new LuaTable();

        final LuaTable displayTable = new LuaTable();
        displayTable.set("clear", new OneArgFunction() {
            @Override
            public LuaValue call(LuaValue arg) {
                int color = arg.checkint();
                display.eraseColor(palette[color]);

                return null;
            }
        });
        displayTable.set("setPixel", new ThreeArgFunction() {
            @Override
            public LuaValue call(LuaValue arg1, LuaValue arg2, LuaValue arg3) {
                int x = arg1.checkint();
                int y = arg2.checkint();
                int color = arg3.checkint();
                display.setPixel(x, y, palette[color]);
                return null;
            }
        });
        retroboy.set("display", displayTable);

        vm.set("retroboy", retroboy);

        InputStream main = filesystem.getInputStream(filesystem.getEntry("main.lua"));
        vm.load(main, "main.lua", "t", vm).call();
    }

    public Bitmap getDisplay() {
        return display;
    }

    public void draw() {
        vm.get("retroboy").get("draw").checkfunction().call();
    }

    public void update(double dt) {
        vm.get("retroboy").get("update").checkfunction().call(LuaDouble.valueOf(dt));
    }
}
