import crc32 from "../config-tool-web/crc.js";

const NUS_SERVICE_UUID = "6e400001-b5a3-f393-e0a9-e50e24dcca9e";
const NUS_RX_UUID = "6e400002-b5a3-f393-e0a9-e50e24dcca9e";

const SLIP_END = 0xc0;
const SLIP_ESC = 0xdb;
const SLIP_ESC_END = 0xdc;
const SLIP_ESC_ESC = 0xdd;

const statusEl = document.querySelector("#status");
const connectButton = document.querySelector("#connect");
const connectFilteredButton = document.querySelector("#connect-filtered");
const disconnectButton = document.querySelector("#disconnect");
const pairButton = document.querySelector("#pair");
const descriptorSelect = document.querySelector("#descriptor");

let device;
let server;
let rxCharacteristic;
let report = neutralReport();
let encryptedLinkReady = false;
let reportDirty = false;
let writeInFlight = false;

function setStatus(message) {
    statusEl.textContent = message;
}

function neutralReport() {
    return new Uint8Array([0x00, 0x00, 0x0f, 0x80, 0x80, 0x80, 0x80, 0x00]);
}

function setConnectedState(connected) {
    connectButton.disabled = connected;
    connectFilteredButton.disabled = connected;
    disconnectButton.disabled = !connected;
    pairButton.disabled = !connected;
}

function formatWriteError(error) {
    const message = error.message || String(error);

    if (error.name === "NetworkError" || /encrypt|security|auth|pair/i.test(message)) {
        return `${message}. The firmware requires an encrypted BLE link; accept the browser or OS pairing prompt, then try again.`;
    }

    if (error.name === "NotAllowedError") {
        return `${message}. Bluetooth access or pairing was cancelled.`;
    }

    return message;
}

function framePacket(payload) {
    const packet = new Uint8Array(payload.length + 4);
    packet.set(payload);

    const view = new DataView(packet.buffer);
    const crc = crc32(new DataView(payload.buffer, payload.byteOffset, payload.byteLength), payload.length);
    view.setUint32(payload.length, crc, true);

    const framed = [SLIP_END];
    for (const byte of packet) {
        if (byte === SLIP_END) {
            framed.push(SLIP_ESC, SLIP_ESC_END);
        } else if (byte === SLIP_ESC) {
            framed.push(SLIP_ESC, SLIP_ESC_ESC);
        } else {
            framed.push(byte);
        }
    }
    framed.push(SLIP_END);
    return new Uint8Array(framed);
}

async function sendCurrentReport(requireResponse = false) {
    if (!rxCharacteristic) {
        setStatus("Not connected");
        return;
    }

    const payload = new Uint8Array(4 + report.length);
    payload[0] = 1;
    payload[1] = Number(descriptorSelect.value);
    payload[2] = report.length;
    payload[3] = 1;
    payload.set(report, 4);

    const framed = framePacket(payload);
    const useResponse = requireResponse || !encryptedLinkReady;
    const t0 = performance.now();

    if (useResponse && rxCharacteristic.writeValueWithResponse) {
        await rxCharacteristic.writeValueWithResponse(framed);
        encryptedLinkReady = true;
    } else if (rxCharacteristic.writeValueWithoutResponse) {
        await rxCharacteristic.writeValueWithoutResponse(framed);
    } else if (rxCharacteristic.writeValue) {
        await rxCharacteristic.writeValue(framed);
    } else if (rxCharacteristic.writeValueWithResponse) {
        await rxCharacteristic.writeValueWithResponse(framed);
        encryptedLinkReady = true;
    }

    const elapsedMs = performance.now() - t0;
    const mode = useResponse ? "encrypted write" : "fast write";
    setStatus(`Sent ${report.length} byte report (${mode}, ${elapsedMs.toFixed(1)} ms) to ${device.name || "HID Remapper"}`);
}

async function flushReportWrites(requireResponse = false) {
    writeInFlight = true;
    try {
        while (reportDirty) {
            reportDirty = false;
            const useResponse = requireResponse || !encryptedLinkReady;
            await sendCurrentReport(useResponse);
            requireResponse = false;
        }
    } catch (error) {
        if (requireResponse || !encryptedLinkReady) {
            encryptedLinkReady = false;
        }
        setStatus(formatWriteError(error));
    } finally {
        writeInFlight = false;
        if (reportDirty) {
            await flushReportWrites(false);
        }
    }
}

function queueCurrentReport(requireResponse = false) {
    reportDirty = true;
    if (writeInFlight) {
        return Promise.resolve();
    }
    return flushReportWrites(requireResponse);
}

function onDisconnected() {
    rxCharacteristic = undefined;
    server = undefined;
    encryptedLinkReady = false;
    reportDirty = false;
    writeInFlight = false;
    setConnectedState(false);
    setStatus("Disconnected");
}

async function connectWithOptions(options) {
    try {
        if (!navigator.bluetooth) {
            setStatus("Web Bluetooth is not available in this browser");
            return;
        }

        setStatus("Opening Bluetooth picker...");
        device = await navigator.bluetooth.requestDevice(options);
        setStatus(`Selected ${device.name || device.id}; connecting GATT...`);
        device.addEventListener("gattserverdisconnected", onDisconnected);

        server = await device.gatt.connect();
        setStatus(`Connected to ${device.name || device.id}; opening NUS service...`);
        const service = await server.getPrimaryService(NUS_SERVICE_UUID);
        setStatus(`NUS service found on ${device.name || device.id}; opening write characteristic...`);
        rxCharacteristic = await service.getCharacteristic(NUS_RX_UUID);

        encryptedLinkReady = false;
        setConnectedState(true);
        setStatus(`Connected to ${device.name || "HID Remapper"}. Use Pair / Send Neutral to trigger encryption, then reports use fast writes.`);
    } catch (error) {
        setStatus(error.message || String(error));
    }
}

connectButton.addEventListener("click", async () => {
    await connectWithOptions({
        acceptAllDevices: true,
        optionalServices: [NUS_SERVICE_UUID],
    });
});

connectFilteredButton.addEventListener("click", async () => {
    await connectWithOptions({
        filters: [{ services: [NUS_SERVICE_UUID] }],
        optionalServices: [NUS_SERVICE_UUID],
    });
});

disconnectButton.addEventListener("click", () => {
    if (device?.gatt?.connected) {
        device.gatt.disconnect();
    }
});

pairButton.addEventListener("click", async () => {
    report = neutralReport();
    encryptedLinkReady = false;
    await queueCurrentReport(true);
});

document.querySelector("#neutral").addEventListener("click", async () => {
    report = neutralReport();
    for (const slider of document.querySelectorAll("input[type='range']")) {
        slider.value = 128;
    }
    await queueCurrentReport();
});

for (const button of document.querySelectorAll("[data-button]")) {
    const bit = Number(button.dataset.button);
    button.addEventListener("click", async () => {
        report[0] |= 1 << bit;
        await queueCurrentReport();
        report[0] &= ~(1 << bit);
        await queueCurrentReport();
    });
}

for (const button of document.querySelectorAll("[data-hat]")) {
    button.addEventListener("click", async () => {
        report[2] = Number(button.dataset.hat);
        await queueCurrentReport();
    });
}

for (const [selector, index] of [
    ["#axis-x", 3],
    ["#axis-y", 4],
    ["#axis-z", 5],
    ["#axis-rz", 6],
]) {
    document.querySelector(selector).addEventListener("input", async (event) => {
        report[index] = Number(event.target.value);
        await queueCurrentReport();
    });
}
