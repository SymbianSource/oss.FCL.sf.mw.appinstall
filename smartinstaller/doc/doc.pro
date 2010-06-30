defineReplace(targetPath) {
    return($$replace(1, /, $$QMAKE_DIR_SEP))
}

QDOC_BIN = $$targetPath($$[QT_INSTALL_BINS]/qdoc3)
HELPGENERATOR = $$targetPath($$[QT_INSTALL_BINS]/qhelpgenerator)

equals(QMAKE_DIR_SEP, /) {   # unix, mingw+msys
    QDOC = SRCDIR=$$PWD OUTDIR=$$OUT_PWD/html $$QDOC_BIN
} else:win32-g++* {   # just mingw
    # The lack of spaces in front of the && is necessary!
    QDOC = set SRCDIR=$$PWD&& set OUTDIR=$$OUT_PWD/html&& $$QDOC_BIN
} else {   # nmake
    QDOC = set SRCDIR=$$PWD $$escape_expand(\n\t) \
           set OUTDIR=$$OUT_PWD/html $$escape_expand(\n\t) \
           $$QDOC_BIN
}

QHP_FILE = $$OUT_PWD/html/smartinstaller.qhp
QCH_FILE = $$PWD/smartinstaller.qch

HELP_DEP_FILES +=$$PWD/smartinstaller.qdoc \
                 $$PWD/smartinstaller.qdocconf \
                 $$PWD/smartinstaller-online.qdocconf

html_docs.commands = $$QDOC $$PWD/smartinstaller.qdocconf
html_docs.depends += $$HELP_DEP_FILES
html_docs.files = $$QHP_FILE

html_docs_online.commands = $$QDOC $$PWD/smartinstaller-online.qdocconf
html_docs_online.depends += $$HELP_DEP_FILES
html_docs_online.files = $$QHP_FILE

qch_docs.commands = $$HELPGENERATOR -o \"$$QCH_FILE\" $$QHP_FILE
qch_docs.depends += html_docs
qch_docs.files = $$QCH_FILE

docs.depends = qch_docs
docs_online.depends = html_docs_online
QMAKE_EXTRA_TARGETS += html_docs qch_docs docs html_docs_online docs_online

OTHER_FILES = $$HELP_DEP_FILES
