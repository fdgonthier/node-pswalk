var assert = require("assert");
var sysinfo = require("./index.js");

describe("sysinfo", function () {
    describe("#meminfo", function () {
        it("should return a non-empty value", function () {
            var r = sysinfo.meminfo();
            assert.ok(r != null);
            assert.ok(Object.keys(r).length > 0);
        });
    });

    describe("#cpuinfo", function () {
        it("should return a non-empty value", function () {
            var r = sysinfo.cpuinfo();
            assert.ok(r != null);
            assert.ok(Object.keys(r).length > 0);
        });
    });

    describe("#pswalk", function () {
        it("should allow for scanning processes", function () {
            var p = [];

            sysinfo.pswalk(function (proc) {
                assert.ok(Object.keys(proc).length > 0);
                p = p.concat([proc]);
            });

            assert.ok(p != []);
            assert.ok(p.length > 0);
        });
    });
});