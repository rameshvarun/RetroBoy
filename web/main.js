var JSZip = require('jszip');
var JSZipUtils = require('jszip-utils');

window.retroboy = {
  activate: function(canvas, url) {
    // Load ROM.
    var fs = new Promise(function(resolve, reject) {
      JSZipUtils.getBinaryContent(url, function(err, data) {
        if (err) {
          reject(err);
        } else {
          resolve(new JSZip(data));
        }
      });
    });

    var palette = ["#0f380f", "#306230", "#8bac0f", "#9bbc0f"];

    fs.then(function(files) {
      console.log("Mounted ROM.");

      var ctx = canvas.getContext('2d');
      var PIXEL_WIDTH = canvas.width / 160;
      var PIXEL_HEIGHT = canvas.height / 144;

      function clear() {
        var color = L.tonumber(1);
        ctx.fillStyle = palette[color];
        ctx.fillRect(0, 0, canvas.width, canvas.height);
        return 0;
      }

      function setPixel() {
      	// TODO: Something is screwed up here:
        var x = L.tonumber(2);
        var y = L.tonumber(3);
        var color = L.tonumber(4);

        ctx.fillStyle = palette[color];
        ctx.fillRect(x * PIXEL_WIDTH, y * PIXEL_HEIGHT, PIXEL_WIDTH, PIXEL_HEIGHT);

        return 0;
      }

      // Create retroboy table.
      L.createtable(0, 1);{
      // Create display table.
      L.createtable(0, 2);

      L.pushjs(clear);
      L.setfield(-2, "clear");

      L.pushjs(setPixel);
      L.setfield(-2, "setPixel");

      L.setfield(-2, "display");
      }
      L.setglobal("retroboy");

      var code = files.file("main.lua").asText();
      L.execute(code);

      var lastTime = (new Date()).getTime();
      function step() {
        L.getglobal("retroboy");
        L.getfield(-1, "draw");
        L.remove(-2);
        if (L.pcall(0, 0, 0)) {
          console.error("Error drawing screen: " + L.tostring(-1));
        }

        var dt = ((new Date()).getTime() - lastTime) / 1000.0;

        L.getglobal("retroboy");
        L.getfield(-1, "update");
        L.remove(-2);
        L.pushnumber(dt);
        if (L.pcall(1, 0, 0)) {
          console.error("Error updating screen: " + L.tostring(-1));
        }
        lastTime = (new Date()).getTime();

        window.requestAnimationFrame(step);
      }
      step();
    });
  }
};
