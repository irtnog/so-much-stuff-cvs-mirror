/^[^/]*$/n
/\/V/ {
    /^[^/]*E\// {
	s@\([^/]*\)E\([/A-Z]*\)/V@\1IVE\
\1E\2@; P; D
    }
    s@\([^/]*\)\([/A-Z]*\)/V@\1IVE\
\1\2@; P; D
}
/\/N/ {
    /^[^/]*E\// {
	s@\([^/]*\)E\([/A-Z]*\)/N@\1ION\
\1E\2@; P; D
    }
    /^[^/]*Y\// {
	s@\([^/]*\)Y\([/A-Z]*\)/N@\1ICATION\
\1Y\2@; P; D
    }
    s@\([^/]*\)\([/A-Z]*\)/N@\1EN\
\1\2@; P; D
}
/\/X/ {
    /^[^/]*E\// {
	s@\([^/]*\)E\([/A-Z]*\)/X@\1IONS\
\1E\2@; P; D
    }
    /^[^/]*Y\// {
	s@\([^/]*\)Y\([/A-Z]*\)/X@\1ICATIONS\
\1Y\2@; P; D
    }
    s@\([^/]*\)\([/A-Z]*\)/X@\1ENS\
\1\2@; P; D
}
/\/H/ {
    /^[^/]*Y\// {
	s@\([^/]*\)Y\([/A-Z]*\)/H@\1IETH\
\1Y\2@; P; D
    }
    s@\([^/]*\)\([/A-Z]*\)/H@\1TH\
\1\2@; P; D
}
/\/Y/ {
    s@\([^/]*\)\([/A-Z]*\)/Y@\1LY\
\1\2@; P; D
}
/\/G/ {
    /^[^/]*E\// {
	s@\([^/]*\)E\([/A-Z]*\)/G@\1ING\
\1E\2@; P; D
    }
    s@\([^/]*\)\([/A-Z]*\)/G@\1ING\
\1\2@; P; D
}
/\/J/ {
    /^[^/]*E\// {
	s@\([^/]*\)E\([/A-Z]*\)/J@\1INGS\
\1E\2@; P; D
    }
    s@\([^/]*\)\([/A-Z]*\)/J@\1INGS\
\1\2@; P; D
}
/\/D/ {
    /^[^/]*E\// {
	s@\([^/]*\)\([/A-Z]*\)/D@\1D\
\1\2@; P; D
    }
    /^[^/]*[^AEIOU]Y\// {
	s@\([^/]*\)Y\([/A-Z]*\)/D@\1IED\
\1Y\2@; P; D
    }
    s@\([^/]*\)\([/A-Z]*\)/D@\1ED\
\1\2@; P; D
}
/\/T/ {
    /^[^/]*E\// {
	s@\([^/]*\)\([/A-Z]*\)/T@\1ST\
\1\2@; P; D
    }
    /^[^/]*[^AEIOU]Y\// {
	s@\([^/]*\)Y\([/A-Z]*\)/T@\1IEST\
\1Y\2@; P; D
    }
    s@\([^/]*\)\([/A-Z]*\)/T@\1EST\
\1\2@; P; D
}
/\/R/ {
    /^[^/]*E\// {
	s@\([^/]*\)\([/A-Z]*\)/R@\1R\
\1\2@; P; D
    }
    /^[^/]*[^AEIOU]Y\// {
	s@\([^/]*\)Y\([/A-Z]*\)/R@\1IER\
\1Y\2@; P; D
    }
    s@\([^/]*\)\([/A-Z]*\)/R@\1ER\
\1\2@; P; D
}