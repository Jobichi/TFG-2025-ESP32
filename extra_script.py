Import("env")
env.Replace(LINKFLAGS=["-Wl,--gc-sections"])
