package pl.poznan.put.cie.ptm.dockercontrol.service.service

import pl.poznan.put.cie.ptm.dockercontrol.service.docker.model.Container
import pl.poznan.put.cie.ptm.dockercontrol.service.docker.model.Image
import pl.poznan.put.cie.ptm.dockercontrol.service.docker.model.Stats
import java.lang.StringBuilder

object DataSerializer {
    fun toContainerArray(containers: List<Container>): String {
        val builder = StringBuilder()
        containers.forEach { builder.append("${toContainer(it)};") }
        return builder.toString()
    }

    private fun toContainer(container: Container) =
        "${container.id}," +
        "${toName(container.names)}," +
        "${container.image}," +
        "${container.state}," +
        toStatus(container.status)

    private fun toName(names: List<String>) = names.first().substringAfter('/')

    private fun toStatus(status: String) = status
        .replace(" weeks", "w")
        .replace("weeks", "w")
        .replace(" week", "w")
        .replace("week", "w")
        .replace(" days", "d")
        .replace("days", "d")
        .replace(" day", "d")
        .replace("day", "d")
        .replace(" hours", "h")
        .replace("hours", "h")
        .replace(" hour", "h")
        .replace("hour", "h")
        .replace(" minutes", "m")
        .replace("minutes", "m")
        .replace(" minute", "m")
        .replace("minute", "m")
        .replace(" seconds", "s")
        .replace("seconds", "s")
        .replace(" second", "s")
        .replace("second", "s")
        .replace(" ago", "")
        .replace("ago", "")
        .replace("Exited ", "")
        .replace("Up", "up")
        .replace(" About", " ~")
        .replace(" an", "")

    fun toImageArray(images: List<Image>): String {
        val builder = StringBuilder()
        images.forEach { builder.append("${toImage(it)};") }
        return builder.toString()
    }

    private fun toImage(image: Image) = image.repoTags.first()

    private const val MEGA = 1_000_000

    fun toStats(stats: Stats) =
            "${stats.running}," +
            "${stats.paused}," +
            "${stats.stopped}," +
            "${stats.images}," +
            "${stats.cpu}," +
            "${stats.memory / MEGA}"
}