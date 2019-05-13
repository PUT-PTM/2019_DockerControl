package pl.poznan.put.cie.ptm.dockercontrol.service.docker.model

import com.fasterxml.jackson.annotation.JsonProperty

data class Stats (
    @JsonProperty("ContainersRunning")
    val running: Int,
    @JsonProperty("ContainersPaused")
    val paused: Int,
    @JsonProperty("ContainersStopped")
    val stopped: Int,
    @JsonProperty("Images")
    val images: Int,
    @JsonProperty("NCPU")
    val cpu: Int,
    @JsonProperty("MemTotal")
    val memory: Int
)