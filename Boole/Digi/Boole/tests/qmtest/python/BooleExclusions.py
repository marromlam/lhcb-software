from GaudiTest import normalizeExamples, RegexpReplacer, LineSkipper

preprocessor = normalizeExamples + \
    RegexpReplacer(when = "Connected to database",
        orig = r"(\"sqlite_file:)(.*/)([\w.]+/[0-9A-Z_]{1,8}\")",
        repl = r"\1\3") + \
    LineSkipper(["/PARAM/ParamFiles/"]) + \
    LineSkipper(["INFO Using TAG", "TimingAuditor.T", "RootDBase.open"]) + \
    LineSkipper(["INFO Connecting to database"]) + \
    LineSkipper(["INFO Disconnect from database after being idle"])
