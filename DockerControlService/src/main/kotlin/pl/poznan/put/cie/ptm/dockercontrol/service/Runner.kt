package pl.poznan.put.cie.ptm.dockercontrol.service

import io.ktor.util.KtorExperimentalAPI
import pl.poznan.put.cie.ptm.dockercontrol.service.app.DockerControl

@KtorExperimentalAPI
fun main(args: Array<String>) = DockerControl.run()
