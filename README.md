# HTTPServer

Servidor HTTP/1.0 simple implementado en C, diseñado para servir archivos estáticos y compliance con
RFC 1945

---


## Requisitos

* GCC (compilador de C)
* GNU Make
* Sistema Linux / Unix-like
* `gdb` (opcional, para depuración)

---

## Compilación

```bash
make
```

El binario se genera en:

```text
bin/servidor
```

Para limpiar los archivos compilados:

```bash
make clean
```

---

## Uso

### Iniciar el servidor

```bash
./bin/servidor 3490
```

Por defecto, el servidor escucha en el puerto **3490**.

---

### Acceder desde un navegador

```text
http://localhost:3490/
```

---

### Usar `curl`

```bash
curl http://localhost:3490/
curl http://localhost:3490/index.html
```

Ver headers de respuesta:

```bash
curl -i http://localhost:3490/
```

---

### Probar manualmente con `netcat`

Petición simple:

```bash
printf "GET / HTTP/1.0\r\n\r\n" | nc localhost 3490
```

Modo interactivo:

```bash
nc localhost 3490
GET /index.html HTTP/1.0

```

---

## Arquitectura


### `server.c`

* Creación y configuración del socket TCP
* `bind`, `listen` y `accept`
* Loop principal de aceptación de conexiones
* Delegación del manejo HTTP

### `http_handler.c`

* Recepción y parsing de la request
* Validación de método, URI y versión
* Resolución de rutas
* Coordinación de respuestas HTTP

### `utils.c`

Funciones auxiliares reutilizables:

* `recv_headers()` — recepción completa de headers HTTP
* `parsear_peticion()` — parsing de la request line
* `validar_ruta()` — sanitización y validación de paths
* `servir_archivo()` — envío de archivos con headers HTTP
* `enviar_error()` — generación de respuestas de error
* `enviar_todo()` — envío confiable de buffers (manejo de envíos parciales)
* `obtener_tipo_mime()` — detección de MIME type por extensión
* `get_fecha()` — fecha en formato RFC 1123

---

## Seguridad


Ejemplo de petición bloqueada:

```bash
curl http://localhost:3490/../../../etc/passwd
# → 403 Forbidden
```

---


## RFC 1945 (HTTP/1.0) — Estado de cumplimiento


### Pendiente (RFC 1945)

* Método `HEAD`
* Header `Last-Modified`
* Conditional GET (`If-Modified-Since`)

---

## Ejemplos

### Servir una página HTML

```bash
echo "<h1>Hola</h1>" > public/test.html
curl http://localhost:3490/test.html
```

---

### Manejo de errores

```bash
# 404
curl -i http://localhost:3490/noexiste.html

# 403 (path traversal)
curl -i http://localhost:3490/../../../etc/passwd
```

---

## Limitaciones conocidas

* Soporte exclusivo para IPv4
* Conexiones secuenciales (single-thread)
* Tamaño máximo de request: 4096 bytes
* No soporta HTTP/1.1
* No soporta métodos distintos de GET

---

## Referencias

* RFC 1945 — HTTP/1.0
* RFC 1123 — Date and Time Specification
* Beej’s Guide to Network Programming

---

