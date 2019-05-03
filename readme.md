# DockerControl

## overview
tba

## description
### server
tba

### stm
tba

### communication
#### protocol
**packet:** `$<sessionId:uint8[3]>!<bodySize:uint12[4]!<cmd:char[n]>!<data?:char[n]>#`
#### commands
| CMD | data from STM | data from server | notes|
|-----|---------------|------------------|------|
| READ | - | packet with new session id and empty data | Signals that STM is ready |
| ACKN | - | - | Acknowledge or ping |
| CALL | - | | All containers |
| CATC | - | | Active containers |
| CSTS | [id] | | Stats of container |
| CSRT | [id] | [success],[id] | Start container |
| CSTP | [id] | [success],[id] | Stop container |
| CRST | [id] | [success],[id] | Restart container |
| CRMV | [id] | [success],[id] | Remove container |
| CCRT | [image] | [success] | Create container from image |
| IALL | - | | All images |
| SSTS | - | | Statistics of system |
| ALRT | *sent only from server* | | Alerts STM |

*[id] – id of a container, [success] – 1 if true, 0 if false, [image] – image name with tag (i.e. mongo:latest)*

## tools
### server
language:  **Kotlin** with [*Ktor*](https://ktor.io/) for networking and [*Jackson*](https://github.com/FasterXML/jackson) for parsing JSON data

other technologies:
* **Docker** for deploying
* **Gradle** for build automation and dependency managment

### stm
language:  **C** with *HAL*

other technologies:
* STM Cube MX
* openOCD

## how to compile & run

### server
#### run locally
tba
#### run on Docker
tba

### stm
tba

## future improvements
tba

## attributions
tba

## license
tba

## credits
The project is made by Jakub Riegel & Maciej Stosik

The project was conducted during the Microprocessor Lab course held by the Institute of Control and Information Engineering, Poznan University of Technology. Supervisor: Tomasz Mańkowski
