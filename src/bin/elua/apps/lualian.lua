-- Lualian application
-- for use with Elua

print("lualian 1")
local lualian = require("lualian")
print("lualian 2")
local  getopt = require("getopt")
print("lualian 3")

local gen_file = function(opts, i, fname)
    local printv  = print
    printv("Generating for file: " .. fname)
    local ofile   = opts["o"] and opts["o"][i] or nil
    local fstream = io.stdout
    if ofile then
        printv("  Output file: " .. ofile)
        fstream = io.open(ofile, "w")
        if not fstream then
            error("Cannot open output file: " .. ofile)
        end
    else
        printv("  Output file: printing to stdout...")
    end
    print("lualian generating")
    lualian.generate(fname, fstream)
    print("lualian generated")
end
print("lualian 4")
getopt.parse {
    usage = "Usage: %prog [OPTIONS] file1.eo file2.eo ... fileN.eo",
    args  = arg,
    descs = {
        { category = "General" },

        { "h", "help", nil, help = "Show this message.", metavar = "CATEGORY",
            callback = getopt.help_cb(io.stdout)
        },
        { "v", "verbose", false, help = "Be verbose." },

        { category = "Generator" },

        { "I", "include", true, help = "Include a directory.", metavar = "DIR",
            list = {}
        },
        { "o", "output", true, help = "Specify output file name(s), by "
            .. "default goes to stdout.",
            list = {}
        }
    },
    error_cb = function(parser, msg)
        io.stderr:write(msg, "\n")
        getopt.help(parser, io.stderr)
    end,
    done_cb = function(parser, opts, args)
        print("lualian done cb")
        if not opts["h"] then
            for i, v in ipairs(opts["I"] or {}) do
                print("lualian include")
                lualian.include_dir(v)
            end
            print("lualian included")
            if os.getenv("EFL_RUN_IN_TREE") then
                lualian.system_directory_scan()
            end
            print("lualian load eot")
            lualian.load_eot_files()
            print("lualian gen")
            for i, fname in ipairs(args) do
                print("lualian gen step")
                gen_file(opts, i, fname)
            end
            print("lualian gend")
        end
    end
}
print("lualian 5")
return true
