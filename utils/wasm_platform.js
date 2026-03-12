//
// Wasm platform
// -------------
//
// This is a light wasm platform implementation that does not
// require libc. It implements the micro-platform.h interface.
//
// It is meant to be used as a light alternative to emscriptem.
//

// Helper function to get a string from a pointer in memory
function getString(ptr, memory) {
    const bytes = new Uint8Array(memory.buffer, ptr);
    let len = 0;
    while (bytes[len] !== 0) len++;
    return new TextDecoder().decode(bytes.slice(0, len));
}

const canvas = document.getElementById('canvas');
const ctx    = canvas.getContext('2d');

let logStr = ""
let engineStartTime = 0;
const importObject = {
    env: {
        js_platform_init: (title_ptr, width, height) => {
            console.log("Initialized platform");
            engineStartTime = performance.now()
            return 0;
        },
        js_platform_terminate: () => {
            console.log("Terminated platform");
            return 0;
        },
        js_platform_draw_frame: (data_ptr, width, height) => {
            const byteLen = width * height * 4;
            const pixels = new Uint8ClampedArray(
                wasmInstance.exports.memory.buffer, 
                data_ptr, 
                byteLen
            );
            
            const imageData = new ImageData(pixels, width, height);
            ctx.putImageData(imageData, 0, 0);
        },
        js_platform_sleep_ms: (ms) => {
        },
        js_platform_get_ticks_ms: () => {
            return performance.now() - engineStartTime;
        },
        js_platform_is_key_down: (w3c_keycode_ptr) => {
            return 0;
        },
        js_platform_print: (msg_ptr) => {
            const str = getString(msg_ptr, wasmInstance.exports.memory);
            logStr += str;
            if (str.includes('\n'))
            {
                console.log("C: ", logStr);
                const len = logStr.length;
                logStr = "";
                return len;
            }
            return 0
        },
    },
}

let wasmInstance;
WebAssembly.instantiateStreaming(fetch("{{{ MODULE_NAME }}}.wasm"), importObject).then(
    (obj) => {
        wasmInstance = obj.instance;

        if (!wasmInstance.exports.micro_app_setup())
            return;
        
        function loop() {
            if (!wasmInstance.exports._micro_app_loop(0))
                return;
            
            requestAnimationFrame(loop);
        }
        requestAnimationFrame(loop);
        
        wasmInstance.exports.micro_app_cleanup();
    }
);
