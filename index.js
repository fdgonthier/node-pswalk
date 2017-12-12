const path = require("path");

/*
 * Copied from node-posix.
 */
function load_extension() {
    var ext_dirs = [
        "build/bindings", "build/Release", "out/Release", "Release",
        "build/Debug", "out/Debug"
    ], i;
    for (i in ext_dirs) {
        try {
            return require(path.join(__dirname, ext_dirs[i], "pswalk"));

        } catch (error) {
            if (!/Cannot find module/.test(error.message))
                throw error;
        }
    }
    throw new Error("unable to load the pswalk extension module");
}

module.exports = load_extension();

