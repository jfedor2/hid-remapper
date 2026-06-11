import { createReadStream, existsSync, statSync } from "node:fs";
import { createServer } from "node:http";
import { extname, join, normalize } from "node:path";

const root = normalize(join(import.meta.dirname, ".."));
const port = Number(process.env.PORT || 8088);
const host = "127.0.0.1";

const contentTypes = new Map([
    [".html", "text/html; charset=utf-8"],
    [".js", "text/javascript; charset=utf-8"],
    [".css", "text/css; charset=utf-8"],
    [".json", "application/json; charset=utf-8"],
]);

createServer((request, response) => {
    const url = new URL(request.url || "/", `http://${host}:${port}`);
    const path = normalize(join(root, decodeURIComponent(url.pathname)));

    if (!path.startsWith(root) || !existsSync(path) || !statSync(path).isFile()) {
        response.writeHead(404, { "Content-Type": "text/plain; charset=utf-8" });
        response.end("Not found\n");
        return;
    }

    response.writeHead(200, {
        "Content-Type": contentTypes.get(extname(path)) || "application/octet-stream",
    });
    createReadStream(path).pipe(response);
}).listen(port, host, () => {
    console.log(`Serving http://${host}:${port}/transmitter-ble-web/index.html`);
});
