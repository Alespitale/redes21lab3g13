# Laboratorio 3: Capa de Transporte

## Analisis

Analisis sobre el impacto de la tasa de transferencia de datos
y memoria de buffers en el trafico de red.

### Modelo de Colas

Consta de tres módulos simples:
1. Generator (crea y transmite paquetes con intervalos dados por una
distribución exponencial de media configurable).
2. Queue (es capaz de atenderlos bajo una misma distribución con un máximo
de 100 paquetes por vez).
3. Sink (toma métricas de la demora de entrega de los paquetes)

## Nuevos módulos agregados

Para el estudio y realizacion de este laboratorio se generaron dos modulos
adicionales, denominados `NodeTx` y `NodeRX`, compuestos como sigue:

- NodeTx: Un Simple Generator y un Simple Queue (`QueueTx`).

- NodeRx: Un Simple Queue (`QueueRx`) y un Simple Sink.

Quedando tres colas a estudiar, `QueueTx`, el
original de la Network `Queue` y `QueueRx`.

## `Caso 1`

En este caso los valores seteados de Data Rate y Delay fueron:

- NodeTx (nodeTx.out) a Queue (queue.in): datarate = 1 Mbps y delay = 100 us

- Queue (queue.out) a NodeRx (nodeRx.in): datarate = 1 Mbps y delay = 100 us

- Queue (queue.out) a Sink (sink.in): datarate = 0.5 Mbps

## Analisis previo:

Viendo la configuracion seteada, podemos ver que habrá un cuello de botella
en el módulo NodeRx debido a que su tasa de transferencia de datos es de
0.5Mbps y la entrada es de 1Mbps, lo que generará que se llene el buffer,
y posteriormente pérdida de paquetes.

## Analisis por intervalos de generacion exponencial

### Intervalo de generacion = 0.1

Paquetes generados y almacenados en el buffer de `NodeTx`: 1979

Paquetes recibidos y almacenados en el buffer de `Network`: 1970
Mostrando una pérdida de 9 paquetes que viajaban desde el buffer de `NodeTx`
hacia el buffer de `Network`.

Paquetes recibidos y almacenados en el buffer de `NodeRx`: 1199
Mostrando un pérdida de 771 paquetes que viajaban desde el buffer de
`Network` hasta el buffer de `NodeRx`. Debido a un buffer overflow en
`NodeRx`, como dijimos en el analisis previo.

Paquetes recibidos en `Sink`: 998

#### Grafico general

![General](imagenes/parte1/caso1/general01.png)

#### NodeTx queue

![NodeTx](imagenes/parte1/caso1/NodeTx01.png)

Aqui podemos ver que se alcanza el pico de 33 paquetes encolados simultaneamente
a los 82s.

#### Network queue

![Queue](imagenes/parte1/caso1/queue01.png)

La cantidad de paquetes encolados se mantiene constante en 1

#### NodeRx queue

![NodeRx](imagenes/parte1/caso1/NodeRx01.png)

En el segundo 42.7 el buffer de NodeRx alcanza su máximo de paquetes almacenables,
lo que implica que empezarán a perderse paquetes.

#### NodeRx delay

![Delay](imagenes/parte1/caso1/delay01.png)

Tiempo que demora un paquete en llegar de origen a destino

#### Promedio de Delay

![NodeRx](imagenes/parte1/caso1/delayAvg01.png)

Aproximadamente hay un promedio de 33s de tiempo que tarda un paquete
en llegar desde origen a destino.

### Intervalo de generacion = 0.166

Paquetes generados y almacenados en el buffer de `NodeTx`: 1190

Paquetes recibidos y almacenados en el buffer de `Network`: 1190

Paquetes recibidos y almacenados en el buffer de `NodeRx`: 1190

Paquetes recibidos en `Sink`: 997

#### Grafico general

![General](imagenes/parte1/caso1/general166.png)

#### NodeTx queue

![NodeTx](imagenes/parte1/caso1/NodeTx166.png)

Se alcanza un máximo de 7 paquetes encolados simultáneamente
a los 137 segundos.

#### Network queue

![Queue](imagenes/parte1/caso1/queue166.png)

La cantidad de paquetes encolados se mantiene constante en 1

#### NodeRx queue

![NodeRx](imagenes/parte1/caso1/NodeRx166.png)

No se llena el buffer de NodeRx

#### NodeRx delay

![Delay](imagenes/parte1/caso1/delay166.png)

Tiempo que demora un paquete en llegar de origen a destino

#### Promedio de Delay

![NodeRx](imagenes/parte1/caso1/delayAvg166.png)

Aproximadamente hay un promedio de 16s de tiempo que tarda un paquete
en llegar desde origen a destino.

### Intervalo de generacion = 0.39

Paquetes generados y almacenados en el buffer de `NodeTx`: 508

Paquetes recibidos y almacenados en el buffer de `Network`: 508

Paquetes recibidos y almacenados en el buffer de `NodeRx`: 508

Paquetes recibidos en `Sink`: 508

Encontramos un punto de inflexión (0.39) donde si bajamos de este número
no llegan todos los paquetes generados a `Sink`.

Por otro lado si bajamos de (0.166) empezaremos a perder paquetes dentro
de los buffers de la network, como se muestra con el valor de intervalo
de generacion (0.1).


## `Caso 2`

En este caso los valores seteados de Data Rate y Delay fueron:

- NodeTx (nodeTx.out) a Queue (queue.in): datarate = 1 Mbps y delay = 100 us

- Queue (queue.out) a NodeRx (nodeRx.in): datarate = 0.5 Mbps y delay = 100 us

- Queue (queue.out) a Sink (sink.in): datarate = 1 Mbps

## Analisis previo:

Viendo la configuracion seteada en este caso, podemos ver que habrá un cuello de botella
en el módulo `Queue` debido a que su tasa de transferencia de datos es de
0.5Mbps y la velocidad de entrada a su buffer es de 1Mbps, lo que generará que se llene el buffer, y posteriormente pérdida de paquetes.

## Analisis por intervalos de generacion exponencial

### Intervalo de generacion = 0.1

Paquetes generados y almacenados en el buffer de `NodeTx`: 1979

Paquetes recibidos y almacenados en el buffer de `Network`: 1199

Mostrando una pérdida de 780 paquetes que viajaban desde el buffer de `NodeTx` hacia el buffer de `Network`, debido a un buffer overflow
en Network queue.

Paquetes recibidos y almacenados en el buffer de `NodeRx`: 999

Se pierden 200 paquetes debido al buffer overflow en Network queue.

Paquetes recibidos en `Sink`: 998


### Intervalo de generacion = 0.166

Paquetes generados y almacenados en el buffer de `NodeTx`: 1190

Paquetes recibidos y almacenados en el buffer de `Network`: 1190

Paquetes recibidos y almacenados en el buffer de `NodeRx`: 997

Se pierden 193 paquetes debido al buffer overflow en Network queue

Paquetes recibidos en `Sink`: 997

`1- ¿Qué diferencia observa entre el caso de estudio 1 y 2?`

La diferencia que se observa es la ubicación de los cuellos de botella.

En el caso 1 encontramos un cuello de botella en el módulo `NodeRx`
dónde el buffer interno recibe paquetes mas rápido de lo que es capaz de despacharlos. Nos encontramos con un problema de flujo de datos

En el caso 2 encontramos un cuello de botella en el módulo `Queue` de Network donde el buffer interno recibe paquetes mas rápido de lo que los
puede despachar. Nos encontramos con un problema de congestión.

`2- ¿Cuál es la fuente limitante en cada uno?`

En el caso 1 la fuente limitante es el data rate en el tramo que va del nodeRx.queue al nodeRx.sink.

En el caso 2 la fuente limitante es el data rate en el tramo que va del Network.queue al NodeRx.queue.