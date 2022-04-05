import crc32 from './crc.js';
import usages from './usages.js';
import examples from './examples.js';

const REPORT_ID_CONFIG = 100;
const UNMAPPED_PASSTHROUGH_FLAG = 0x01;
const STICKY_FLAG = 0x01;
const CONFIG_SIZE = 32;
const CONFIG_VERSION = 2;
const VENDOR_ID = 0xCAFE;
const PRODUCT_ID = 0xBAF2;
const DEFAULT_PARTIAL_SCROLL_TIMEOUT = 1000000;
const DEFAULT_SCALING = 1000;

const SET_CONFIG = 2;
const GET_CONFIG = 3;
const CLEAR_MAPPING = 4;
const ADD_MAPPING = 5;
const GET_MAPPING = 6;
const PERSIST_CONFIG = 7;
const GET_OUR_USAGES = 8
const GET_THEIR_USAGES = 9
const SUSPEND = 10;
const RESUME = 11;

const UINT8 = Symbol('uint8');
const UINT32 = Symbol('uint32');
const INT32 = Symbol('int32');

let device = null;
let modal = null;
let extra_usages = [];
let config = {
    'version': CONFIG_VERSION,
    'unmapped_passthrough': true,
    'partial_scroll_timeout': DEFAULT_PARTIAL_SCROLL_TIMEOUT,
    mappings: [{
        'source_usage': '0x00000000',
        'target_usage': '0x00000000',
        'layer': 0,
        'sticky': false,
        'scaling': DEFAULT_SCALING,
    }]
};
const ignored_usages = new Set([
]);

document.addEventListener("DOMContentLoaded", function () {
    document.getElementById("open_device").addEventListener("click", open_device);
    document.getElementById("load_from_device").addEventListener("click", load_from_device);
    document.getElementById("save_to_device").addEventListener("click", save_to_device);
    document.getElementById("add_mapping").addEventListener("click", add_mapping_onclick);
    document.getElementById("download_json").addEventListener("click", download_json);
    document.getElementById("upload_json").addEventListener("click", upload_json);
    document.getElementById("file_input").addEventListener("change", file_uploaded);

    document.getElementById("load_from_device").disabled = true;
    document.getElementById("save_to_device").disabled = true;

    document.getElementById("partial_scroll_timeout_input").addEventListener("change", partial_scroll_timeout_onchange);
    document.getElementById("unmapped_passthrough_checkbox").addEventListener("change", unmapped_passthrough_onchange);

    setup_examples();
    modal = new bootstrap.Modal(document.getElementById('usage_modal'), {});
    setup_usages_modal();
    set_ui_state();
});

async function open_device() {
    clear_error();
    let success = false;
    const devices = await navigator.hid.requestDevice({
        filters: [{ vendorId: VENDOR_ID, productId: PRODUCT_ID }]
    }).catch((err) => { display_error(err); });
    if (devices !== undefined && devices.length > 0) {
        device = devices[0];
        if (!device.opened) {
            await device.open().catch((err) => { display_error(err + "\nIf you're on Linux, you might need to give yourself permissions to the appropriate /dev/hidraw* device."); });
        }
        success = device.opened;
        if (success) {
            await get_usages_from_device();
            setup_usages_modal();
        }
    }
    document.getElementById("load_from_device").disabled = !success;
    document.getElementById("save_to_device").disabled = !success;
    if (!success) {
        device = null;
    }
}

async function load_from_device() {
    if (device == null) {
        return;
    }
    clear_error();

    try {
        await send_feature_command(GET_CONFIG);
        const [config_version, flags, partial_scroll_timeout, mapping_count] =
            await read_config_feature([UINT8, UINT8, UINT32, UINT32]);
        check_version(config_version);

        config['version'] = config_version;
        config['unmapped_passthrough'] = (flags & UNMAPPED_PASSTHROUGH_FLAG) != 0;
        config['partial_scroll_timeout'] = partial_scroll_timeout;
        config['mappings'] = [];

        for (let i = 0; i < mapping_count; i++) {
            await send_feature_command(GET_MAPPING, [[UINT32, i]]);
            const [target_usage, source_usage, scaling, layer, mapping_flags] =
                await read_config_feature([UINT32, UINT32, INT32, UINT8, UINT8]);
            config['mappings'].push({
                'target_usage': '0x' + target_usage.toString(16).padStart(8, '0'),
                'source_usage': '0x' + source_usage.toString(16).padStart(8, '0'),
                'scaling': scaling,
                'layer': layer,
                'sticky': (mapping_flags & STICKY_FLAG) != 0,
            });
        }

        set_ui_state();
    } catch (e) {
        display_error(e);
    }
}

async function save_to_device() {
    if (device == null) {
        return;
    }
    clear_error();

    try {
        await send_feature_command(SUSPEND);
        await send_feature_command(SET_CONFIG, [
            [UINT8, config['unmapped_passthrough'] ? UNMAPPED_PASSTHROUGH_FLAG : 0],
            [UINT32, config['partial_scroll_timeout']],
        ]);
        await send_feature_command(CLEAR_MAPPING);

        for (const mapping of config['mappings']) {
            await send_feature_command(ADD_MAPPING, [
                [UINT32, parseInt(mapping['target_usage'], 16)],
                [UINT32, parseInt(mapping['source_usage'], 16)],
                [INT32, mapping['scaling']],
                [UINT8, mapping['layer']],
                [UINT8, mapping['sticky'] ? STICKY_FLAG : 0],
            ]);
        }

        await send_feature_command(PERSIST_CONFIG);
        await send_feature_command(RESUME);
    } catch (e) {
        display_error(e);
    }
}

async function get_usages_from_device() {
    try {
        await send_feature_command(GET_CONFIG);
        const [config_version, flags, partial_scroll_timeout, mapping_count, our_usage_count, their_usage_count] =
            await read_config_feature([UINT8, UINT8, UINT32, UINT32, UINT32, UINT32]);
        check_version(config_version);

        let extra_usage_set = new Set();

        for (const [command, rle_count] of [
            [GET_OUR_USAGES, our_usage_count],
            [GET_THEIR_USAGES, their_usage_count]
        ]) {
            let i = 0;
            while (i < rle_count) {
                await send_feature_command(command, [[UINT32, i]]);
                const fields = await read_config_feature([UINT32, UINT32, UINT32, UINT32, UINT32, UINT32]);

                for (let j = 0; j < 3; j++) {
                    const usage = fields[2 * j];
                    const count = fields[2 * j + 1];
                    if (usage != 0) {
                        for (let k = 0; k < count; k++) {
                            const u = '0x' + (usage + k).toString(16).padStart(8, '0');
                            if (!(u in usages) && !ignored_usages.has(u)) {
                                extra_usage_set.add(u);
                            }
                        }
                    }
                }

                i += 3;
            }
        }

        extra_usages = Array.from(extra_usage_set);
        extra_usages.sort();
    } catch (e) {
        display_error(e);
    }
}

function set_config_ui_state() {
    document.getElementById('partial_scroll_timeout_input').value = Math.round(config['partial_scroll_timeout'] / 1000);
    document.getElementById('unmapped_passthrough_checkbox').checked = config['unmapped_passthrough'];
}

function set_mappings_ui_state() {
    clear_children(document.getElementById('mappings'));
    for (const mapping of config['mappings']) {
        add_mapping(mapping);
    }
}

function set_ui_state() {
    set_config_ui_state();
    set_mappings_ui_state();
}

function add_mapping(mapping) {
    const template = document.getElementById("mapping_template");
    const container = document.getElementById("mappings");
    const clone = template.content.cloneNode(true).firstElementChild;
    clone.querySelector(".delete_button").addEventListener("click", delete_mapping(mapping, clone));
    const sticky_checkbox = clone.querySelector(".sticky_checkbox");
    sticky_checkbox.checked = mapping['sticky'];
    sticky_checkbox.addEventListener("change", sticky_onclick(mapping, sticky_checkbox));
    const scaling_input = clone.querySelector(".scaling_input");
    scaling_input.value = mapping['scaling'] / 1000;
    scaling_input.addEventListener("input", scaling_onchange(mapping, scaling_input));
    const layer_dropdown = clone.querySelector(".layer_dropdown");
    layer_dropdown.value = mapping['layer'];
    layer_dropdown.addEventListener("change", layer_onchange(mapping, layer_dropdown));
    const source_button = clone.querySelector(".source_button");
    source_button.innerText = (mapping['source_usage'] in usages) ? usages[mapping['source_usage']]['name'] : mapping['source_usage'];
    source_button.setAttribute('data-hid-usage', mapping['source_usage']);
    source_button.addEventListener("click", show_usage_modal(mapping, 'source', source_button));
    const target_button = clone.querySelector(".target_button");
    target_button.innerText = (mapping['target_usage'] in usages) ? usages[mapping['target_usage']]['name'] : mapping['target_usage'];
    target_button.setAttribute('data-hid-usage', mapping['target_usage']);
    target_button.addEventListener("click", show_usage_modal(mapping, 'target', target_button));
    container.appendChild(clone);
}

function download_json() {
    clear_error();
    let element = document.createElement('a');
    element.setAttribute('href', 'data:application/json,' + encodeURIComponent(JSON.stringify(config, null, 4)));
    element.setAttribute('download', 'hid-remapper-config.json');

    element.style.display = 'none';
    document.body.appendChild(element);

    element.click();

    document.body.removeChild(element);
}

function upload_json() {
    clear_error();
    document.getElementById("file_input").click();
}

function file_uploaded() {
    const reader = new FileReader();
    reader.onload = function (e) {
        try {
            const new_config = JSON.parse(e.target.result);
            check_version(new_config['version']);
            config = new_config;
            set_ui_state();
        } catch (e) {
            display_error(e);
        }
    };

    const file = document.getElementById("file_input").files[0];
    if (file !== undefined) {
        reader.readAsText(file);
    }

    document.getElementById("file_input").value = '';
}

async function send_feature_command(command, fields = []) {
    let buffer = new ArrayBuffer(CONFIG_SIZE);
    let dataview = new DataView(buffer);
    dataview.setUint8(0, CONFIG_VERSION);
    dataview.setUint8(1, command);
    let pos = 2;
    for (const [type, value] of fields) {
        switch (type) {
            case UINT8:
                dataview.setUint8(pos, value);
                pos += 1;
                break;
            case UINT32:
                dataview.setUint32(pos, value, true);
                pos += 4;
                break;
            case INT32:
                dataview.setInt32(pos, value, true);
                pos += 4;
                break;
        }
    }
    add_crc(dataview);

    await device.sendFeatureReport(REPORT_ID_CONFIG, buffer);
}

async function read_config_feature(fields = []) {
    const data_with_report_id = await device.receiveFeatureReport(REPORT_ID_CONFIG);
    const data = new DataView(data_with_report_id.buffer, 1);
    check_crc(data);
    let ret = [];
    let pos = 0;
    for (const type of fields) {
        switch (type) {
            case UINT8:
                ret.push(data.getUint8(pos));
                pos += 1;
                break;
            case UINT32:
                ret.push(data.getUint32(pos, true));
                pos += 4;
                break;
            case INT32:
                ret.push(data.getInt32(pos, true));
                pos += 4;
                break;
        }
    }
    return ret;
}

function clear_error() {
    document.getElementById("error").classList.add("d-none");
}

function display_error(message) {
    document.getElementById("error").innerText = message;
    document.getElementById("error").classList.remove("d-none");
}

function check_crc(data) {
    if (data.getUint32(CONFIG_SIZE - 4, true) != crc32(data, CONFIG_SIZE - 4)) {
        throw new Error('CRC error.');
    }
}

function add_crc(data) {
    data.setUint32(CONFIG_SIZE - 4, crc32(data, CONFIG_SIZE - 4), true);
}

function check_version(config_version) {
    if (config_version != CONFIG_VERSION) {
        throw new Error("Incompatible version.");
    }
}

function clear_children(element) {
    while (element.firstChild) {
        element.removeChild(element.firstChild);
    }
}

function delete_mapping(mapping, element) {
    return function () {
        config['mappings'] = config['mappings'].filter(x => x !== mapping);
        document.getElementById("mappings").removeChild(element);
    };
}

function sticky_onclick(mapping, element) {
    return function () {
        mapping['sticky'] = element.checked;
    };
}

function scaling_onchange(mapping, element) {
    return function () {
        mapping['scaling'] = element.value === '' ? DEFAULT_SCALING : Math.round(parseFloat(element.value) * 1000);
    };
}

function layer_onchange(mapping, element) {
    return function () {
        mapping['layer'] = element.value;
    };
}

function show_usage_modal(mapping, source_or_target, element) {
    return function () {
        document.querySelector('.usage_modal_title').innerText = "Select " + (source_or_target == 'source' ? "input" : "output");
        // XXX it would be better not to do this every time we show the modal
        document.querySelectorAll('.usage_button').forEach((button) => {
            let clone = button.cloneNode(true);
            button.parentNode.replaceChild(clone, button); // to clear existing event listeners
            clone.addEventListener("click", function () {
                let usage = clone.getAttribute('data-hid-usage');
                mapping[source_or_target + '_usage'] = usage;
                element.innerText = usage in usages ? usages[usage]['name'] : usage;
                modal.hide();
            });
        });
        modal.show();
    };
}

function add_mapping_onclick() {
    let new_mapping = {
        'source_usage': '0x00000000',
        'target_usage': '0x00000000',
        'layer': 0,
        'sticky': false,
        'scaling': DEFAULT_SCALING
    };
    config['mappings'].push(new_mapping);
    add_mapping(new_mapping);
}

function setup_usages_modal() {
    let usage_classes = {
        'mouse': document.querySelector('.mouse_usages'),
        'keyboard': document.querySelector('.keyboard_usages'),
        'media': document.querySelector('.media_usages'),
        'other': document.querySelector('.other_usages'),
        'extra': document.querySelector('.extra_usages'),
    };
    for (const [usage_class, element] of Object.entries(usage_classes)) {
        clear_children(element);
    }
    let template = document.getElementById('usage_button_template');
    for (const [usage, usage_def] of Object.entries(usages)) {
        let clone = template.content.cloneNode(true).firstElementChild;
        clone.innerText = usage_def['name'];
        clone.setAttribute('data-hid-usage', usage);
        usage_classes[usage_def['class']].appendChild(clone);
    }
    for (const usage_ of extra_usages) {
        let clone = template.content.cloneNode(true).firstElementChild;
        clone.innerText = usage_;
        clone.setAttribute('data-hid-usage', usage_);
        usage_classes['extra'].appendChild(clone);
    }
}

function partial_scroll_timeout_onchange() {
    let value = document.getElementById('partial_scroll_timeout_input').value;
    if (value === '') {
        value = DEFAULT_PARTIAL_SCROLL_TIMEOUT;
    } else {
        value = Math.round(value * 1000);
    }
    config['partial_scroll_timeout'] = value;
}

function unmapped_passthrough_onchange() {
    config['unmapped_passthrough'] = document.getElementById("unmapped_passthrough_checkbox").checked;
}

function load_example(n) {
    config = structuredClone(examples[n]['config']);
    set_ui_state();
}

function setup_examples() {
    const element = document.getElementById("examples");
    const template = document.getElementById("example_template");
    for (let i = 0; i < examples.length; i++) {
        if (i > 0) {
            element.appendChild(document.createTextNode(', '));
        }
        const clone = template.content.cloneNode(true).firstElementChild;
        clone.innerText = examples[i]['description'];
        clone.addEventListener("click", () => load_example(i));
        element.appendChild(clone);
    }
    element.appendChild(document.createTextNode('.'));
}
