package pl.poznan.put.cie.ptm.dockercontrol.service.commands

enum class CMD {
    // utils
    READ, ACKN, ERRR,

    // containers
    CALL, CATC, CSTS, CSTR, CSTP, CRST, CRMV, CCRT,

    // images
    IALL,

    // system
    SSTS, ALRT
}