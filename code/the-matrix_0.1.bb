SUMMARY = "Matrix demo app for profiling with Linux perf"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=d7810fab7487fb0aad327b76f1be7cd7"

SRC_URI = "file://the-matrix.c"

do_compile () {
        ${CC} -lm -g ${WORKDIR}/the-matrix.c -o ${WORKDIR}/the-matrix
        ${CC} -lm -g -O3 ${WORKDIR}/the-matrix.c -o ${WORKDIR}/the-matrix.opt
}

do_install () {
        install -d ${D}${bindir}
        install -m 0755 ${WORKDIR}/the-matrix ${D}${bindir}/
        install -m 0755 ${WORKDIR}/the-matrix.opt ${D}${bindir}/
}

