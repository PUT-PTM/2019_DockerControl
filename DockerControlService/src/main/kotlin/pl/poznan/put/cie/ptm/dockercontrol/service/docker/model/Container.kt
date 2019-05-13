package pl.poznan.put.cie.ptm.dockercontrol.service.docker.model

import java.time.Instant

data class Container (
        val id: String,
        val names: List<String>,
        val image: String,
        val created: Instant,
        val state: String,
        val status: String
)
