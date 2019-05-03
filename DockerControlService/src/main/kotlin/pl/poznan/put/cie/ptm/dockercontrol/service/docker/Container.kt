package pl.poznan.put.cie.ptm.dockercontrol.service.docker

import java.time.Instant

data class Container (
        val id: String,
        val names: List<String>,
        val created: Instant,
        val state: String,
        val status: String
)
