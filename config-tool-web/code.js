import crc32 from './crc.js';
import usages from './usages.js';
import examples from './examples.js';

const REPORT_ID_CONFIG = 100;
const REPORT_ID_MONITOR = 101;
const STICKY_FLAG = 1 << 0;
const TAP_FLAG = 1 << 1;
const HOLD_FLAG = 1 << 2;
const CONFIG_SIZE = 32;
const CONFIG_VERSION = 9;
const VENDOR_ID = 0xCAFE;
const PRODUCT_ID = 0xBAF2;
const DEFAULT_PARTIAL_SCROLL_TIMEOUT = 1000000;
const DEFAULT_TAP_HOLD_THRESHOLD = 200000;
const DEFAULT_GPIO_DEBOUNCE_TIME = 5;
const DEFAULT_SCALING = 1000;

const NLAYERS = 4;
const NMACROS = 32;
const NEXPRESSIONS = 8;
const MACRO_ITEMS_IN_PACKET = 6;
const IGNORE_AUTH_DEV_INPUTS_FLAG = 1 << 4;

const LAYERS_USAGE_PAGE = 0xFFF10000;
const EXPR_USAGE_PAGE = 0xFFF30000;
const MIDI_USAGE_PAGE = 0xFFF70000;

const RESET_INTO_BOOTSEL = 1;
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
const PAIR_NEW_DEVICE = 12;
const CLEAR_BONDS = 13;
const FLASH_B_SIDE = 14;
const CLEAR_MACROS = 15;
const APPEND_TO_MACRO = 16;
const GET_MACRO = 17;
const INVALID_COMMAND = 18;
const CLEAR_EXPRESSIONS = 19;
const APPEND_TO_EXPRESSION = 20;
const GET_EXPRESSION = 21;
const SET_MONITOR_ENABLED = 22;

const ops = {
    "PUSH": 0,
    "PUSH_USAGE": 1,
    "INPUT_STATE": 2,
    "ADD": 3,
    "MUL": 4,
    "EQ": 5,
    "TIME": 6,
    "MOD": 7,
    "GT": 8,
    "NOT": 9,
    "INPUT_STATE_BINARY": 10,
    "ABS": 11,
    "DUP": 12,
    "SIN": 13,
    "COS": 14,
    "DEBUG": 15,
    "AUTO_REPEAT": 16,
    "RELU": 17,
    "CLAMP": 18,
    "SCALING": 19,
    "LAYER_STATE": 20,
    "STICKY_STATE": 21,
    "TAP_STATE": 22,
    "HOLD_STATE": 23,
    "BITWISE_OR": 24,
    "BITWISE_AND": 25,
    "BITWISE_NOT": 26,
    "PREV_INPUT_STATE": 27,
    "PREV_INPUT_STATE_BINARY": 28,
    "STORE": 29,
    "RECALL": 30,
    "SQRT": 31,
    "ATAN2": 32,
    "ROUND": 33,
}

const opcodes = Object.fromEntries(Object.entries(ops).map(([key, value]) => [value, key]));

const UINT8 = Symbol('uint8');
const UINT32 = Symbol('uint32');
const INT32 = Symbol('int32');

let device = null;
let source_modal = null;
let target_modal = null;
let extra_usages = { 'source': [], 'target': [] };
let config = {
    'version': CONFIG_VERSION,
    'unmapped_passthrough_layers': [0, 1, 2, 3],
    'partial_scroll_timeout': DEFAULT_PARTIAL_SCROLL_TIMEOUT,
    'tap_hold_threshold': DEFAULT_TAP_HOLD_THRESHOLD,
    'gpio_debounce_time_ms': DEFAULT_GPIO_DEBOUNCE_TIME,
    'interval_override': 0,
    'our_descriptor_number': 0,
    'ignore_auth_dev_inputs': false,
    mappings: [{
        'source_usage': '0x00000000',
        'target_usage': '0x00000000',
        'layers': [0],
        'sticky': false,
        'tap': false,
        'hold': false,
        'scaling': DEFAULT_SCALING,
    }],
    macros: [
        [], [], [], [], [], [], [], [], [], [], [], [], [], [], [], [],
        [], [], [], [], [], [], [], [], [], [], [], [], [], [], [], []
    ],
    expressions: [
        '', '', '', '', '', '', '', ''
    ],
};
let monitor_enabled = false;
let monitor_min_val = {};
let monitor_max_val = {};
const ignored_usages = new Set([
]);

document.addEventListener("DOMContentLoaded", function () {
    document.getElementById("open_device").addEventListener("click", open_device);
    document.getElementById("load_from_device").addEventListener("click", load_from_device);
    document.getElementById("save_to_device").addEventListener("click", save_to_device);
    document.getElementById("add_mapping").addEventListener("click", add_mapping_onclick);
    document.getElementById("download_json").addEventListener("click", download_json);
    document.getElementById("upload_json").addEventListener("click", upload_json);
    document.getElementById("flash_firmware").addEventListener("click", flash_firmware);
    document.getElementById("flash_b_side").addEventListener("click", flash_b_side);
    document.getElementById("pair_new_device").addEventListener("click", pair_new_device);
    document.getElementById("clear_bonds").addEventListener("click", clear_bonds);
    document.getElementById("monitor_clear").addEventListener("click", monitor_clear);
    document.getElementById("file_input").addEventListener("change", file_uploaded);

    device_buttons_set_disabled_state(true);

    document.getElementById("partial_scroll_timeout_input").addEventListener("change", partial_scroll_timeout_onchange);
    document.getElementById("tap_hold_threshold_input").addEventListener("change", tap_hold_threshold_onchange);
    document.getElementById("gpio_debounce_time_input").addEventListener("change", gpio_debounce_time_onchange);
    for (let i = 0; i < NLAYERS; i++) {
        document.getElementById("unmapped_passthrough_checkbox" + i).addEventListener("change", unmapped_passthrough_onchange);
    }
    document.getElementById("interval_override_dropdown").addEventListener("change", interval_override_onchange);
    document.getElementById("our_descriptor_number_dropdown").addEventListener("change", our_descriptor_number_onchange);
    document.getElementById("ignore_auth_dev_inputs_checkbox").addEventListener("change", ignore_auth_dev_inputs_onchange);

    document.getElementById("nav-monitor-tab").addEventListener("shown.bs.tab", monitor_tab_shown);
    document.getElementById("nav-monitor-tab").addEventListener("hide.bs.tab", monitor_tab_hide);

    if ("hid" in navigator) {
        navigator.hid.addEventListener('disconnect', hid_on_disconnect);
    } else {
        display_error("Your browser doesn't support WebHID. Try Chrome (desktop version) or a Chrome-based browser.");
    }

    setup_examples();
    source_modal = new bootstrap.Modal(document.getElementById('source_usage_modal'), {});
    target_modal = new bootstrap.Modal(document.getElementById('target_usage_modal'), {});
    setup_macros();
    setup_expressions();
    set_ui_state();
});

async function open_device() {
    clear_error();
    let success = false;
    const devices = await navigator.hid.requestDevice({
        filters: [{ usagePage: 0xFF00, usage: 0x0020 }]
    }).catch((err) => { display_error(err); });
    const config_interface = devices?.find(d => d.collections.some(c => c.usagePage == 0xff00));
    if (config_interface !== undefined) {
        device = config_interface;
        if (!device.opened) {
            await device.open().catch((err) => { display_error(err + "\nIf you're on Linux, you might need to give yourself permissions to the appropriate /dev/hidraw* device."); });
        }
        success = device.opened;
        success &&= await check_device_version();
        if (success) {
            device.addEventListener('inputreport', input_report_received);
            await set_monitor_enabled(monitor_enabled);
            await get_usages_from_device();
            setup_usages_modals();
            bluetooth_buttons_set_visibility(device.productName.includes("Bluetooth"));
        }
    }

    device_buttons_set_disabled_state(!success);

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
        const [config_version, flags, partial_scroll_timeout, mapping_count, our_usage_count, their_usage_count, interval_override, tap_hold_threshold, gpio_debounce_time_ms, our_descriptor_number] =
            await read_config_feature([UINT8, UINT8, UINT32, UINT32, UINT32, UINT32, UINT8, UINT32, UINT8, UINT8]);
        check_received_version(config_version);

        config['version'] = config_version;
        config['unmapped_passthrough_layers'] = mask_to_layer_list(flags & ((1 << NLAYERS) - 1));
        config['partial_scroll_timeout'] = partial_scroll_timeout;
        config['tap_hold_threshold'] = tap_hold_threshold;
        config['gpio_debounce_time_ms'] = gpio_debounce_time_ms;
        config['interval_override'] = interval_override;
        config['our_descriptor_number'] = our_descriptor_number;
        config['ignore_auth_dev_inputs'] = !!(flags & IGNORE_AUTH_DEV_INPUTS_FLAG);
        config['mappings'] = [];

        for (let i = 0; i < mapping_count; i++) {
            await send_feature_command(GET_MAPPING, [[UINT32, i]]);
            const [target_usage, source_usage, scaling, layer_mask, mapping_flags] =
                await read_config_feature([UINT32, UINT32, INT32, UINT8, UINT8]);
            config['mappings'].push({
                'target_usage': '0x' + target_usage.toString(16).padStart(8, '0'),
                'source_usage': '0x' + source_usage.toString(16).padStart(8, '0'),
                'scaling': scaling,
                'layers': mask_to_layer_list(layer_mask),
                'sticky': (mapping_flags & STICKY_FLAG) != 0,
                'tap': (mapping_flags & TAP_FLAG) != 0,
                'hold': (mapping_flags & HOLD_FLAG) != 0,
            });
        }

        config['macros'] = [];

        for (let macro_i = 0; macro_i < NMACROS; macro_i++) {
            let macro = [];
            let i = 0;
            let keep_going = true;
            while (keep_going) {
                await send_feature_command(GET_MACRO, [[UINT32, macro_i], [UINT32, i]]);
                const fields = await read_config_feature([UINT8, UINT32, UINT32, UINT32, UINT32, UINT32, UINT32]);
                const nitems = fields[0];
                const usages_ = fields.slice(1);
                if (nitems < MACRO_ITEMS_IN_PACKET) {
                    keep_going = false;
                }
                if ((macro.length == 0) && (nitems > 0)) {
                    macro = [[]];
                }
                for (const usage of usages_.slice(0, nitems)) {
                    if (usage == 0) {
                        macro.push([]);
                    } else {
                        macro.at(-1).push('0x' + usage.toString(16).padStart(8, '0'));
                    }
                }
                i += MACRO_ITEMS_IN_PACKET;
            }

            config['macros'].push(macro);
        }

        config['expressions'] = [];

        for (let expr_i = 0; expr_i < NEXPRESSIONS; expr_i++) {
            let expression = [];
            let i = 0;
            while (true) {
                await send_feature_command(GET_EXPRESSION, [[UINT32, expr_i], [UINT32, i]]);
                const fields = await read_config_feature([UINT8, UINT8, UINT8, UINT8, UINT8, UINT8, UINT8, UINT8, UINT8, UINT8, UINT8, UINT8, UINT8, UINT8, UINT8, UINT8, UINT8, UINT8, UINT8, UINT8, UINT8, UINT8, UINT8, UINT8, UINT8, UINT8, UINT8, UINT8]);
                const nelems = fields[0];
                if (nelems == 0) {
                    break;
                }
                let elems = fields.slice(1);
                for (let j = 0; j < nelems; j++) {
                    const elem = elems[0];
                    elems = elems.slice(1);
                    if ([ops['PUSH'], ops['PUSH_USAGE']].includes(elem)) {
                        const val = elems[3] << 24 | elems[2] << 16 | elems[1] << 8 | elems[0];
                        elems = elems.slice(4);
                        if (elem == ops['PUSH']) {
                            expression.push(val.toString());
                        } else {
                            expression.push('0x' + val.toString(16).padStart(8, '0'));
                        }
                    } else {
                        expression.push(opcodes[elem].toLowerCase());
                    }
                }
                i += nelems;
            }

            config['expressions'].push(expression.join(' '));
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
        const flags = layer_list_to_mask(config['unmapped_passthrough_layers']) |
            (config['ignore_auth_dev_inputs'] ? IGNORE_AUTH_DEV_INPUTS_FLAG : 0);
        await send_feature_command(SET_CONFIG, [
            [UINT8, flags],
            [UINT32, config['partial_scroll_timeout']],
            [UINT8, config['interval_override']],
            [UINT32, config['tap_hold_threshold']],
            [UINT8, config['gpio_debounce_time_ms']],
            [UINT8, config['our_descriptor_number']],
        ]);
        await send_feature_command(CLEAR_MAPPING);

        for (const mapping of config['mappings']) {
            await send_feature_command(ADD_MAPPING, [
                [UINT32, parseInt(mapping['target_usage'], 16)],
                [UINT32, parseInt(mapping['source_usage'], 16)],
                [INT32, mapping['scaling']],
                [UINT8, layer_list_to_mask(mapping['layers'])],
                [UINT8, (mapping['sticky'] ? STICKY_FLAG : 0)
                    | (mapping['tap'] ? TAP_FLAG : 0)
                    | (mapping['hold'] ? HOLD_FLAG : 0)]
            ]);
        }

        await send_feature_command(CLEAR_MACROS);
        let macro_i = 0;
        for (const macro of config['macros']) {
            if (macro_i >= NMACROS) {
                break;
            }

            const flat_zero_separated = macro.map((x) => x.concat(["0x00"])).flat().slice(0, -1);

            for (let i = 0; i < flat_zero_separated.length; i += MACRO_ITEMS_IN_PACKET) {
                const chunk_size = i + MACRO_ITEMS_IN_PACKET > flat_zero_separated.length
                    ? flat_zero_separated.length - i
                    : MACRO_ITEMS_IN_PACKET;
                await send_feature_command(APPEND_TO_MACRO,
                    [[UINT8, macro_i], [UINT8, chunk_size]].concat(
                        flat_zero_separated
                            .slice(i, i + chunk_size)
                            .map((x) => [UINT32, parseInt(x, 16)])));
            }

            macro_i++;
        }

        await send_feature_command(CLEAR_EXPRESSIONS);
        let expr_i = 0;
        for (const expr of config['expressions']) {
            if (expr_i >= NEXPRESSIONS) {
                break;
            }

            let elems = expr_to_elems(expr);
            while (elems.length > 0) {
                let bytes_left = 24;
                let items_to_send = [];
                let nelems = 0;
                while ((elems.length > 0) && (bytes_left > 0)) {
                    const elem = elems[0];
                    if ([ops["PUSH"], ops["PUSH_USAGE"]].includes(elem[0])) {
                        if (bytes_left >= 5) {
                            items_to_send.push([UINT8, elem[0]]);
                            items_to_send.push([UINT32, elem[1] >>> 0]);
                            bytes_left -= 5;
                            nelems++;
                            elems = elems.slice(1);
                        } else {
                            break
                        }
                    } else {
                        items_to_send.push([UINT8, elem[0]]);
                        bytes_left--;
                        nelems++;
                        elems = elems.slice(1);
                    }
                }
                await send_feature_command(APPEND_TO_EXPRESSION,
                    [[UINT8, expr_i], [UINT8, nelems]].concat(items_to_send));
            }

            expr_i++;
        }

        await send_feature_command(PERSIST_CONFIG);
        await send_feature_command(RESUME);
    } catch (e) {
        display_error(e);
    }
}

async function do_get_usages_from_device(command, rle_count) {
    const extra_usage_set = new Set();
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
                    if (!ignored_usages.has(u)) {
                        extra_usage_set.add(u);
                    }
                }
            }
        }

        i += 3;
    }

    const extra_usages_ = Array.from(extra_usage_set);
    extra_usages_.sort();
    return extra_usages_;
}

async function get_usages_from_device() {
    try {
        await send_feature_command(GET_CONFIG);
        const [config_version, flags, partial_scroll_timeout, mapping_count, our_usage_count, their_usage_count, tap_hold_threshold, gpio_debounce_time_ms, our_descriptor_number] =
            await read_config_feature([UINT8, UINT8, UINT32, UINT32, UINT32, UINT32, UINT32, UINT8, UINT8]);
        check_received_version(config_version);

        extra_usages['target'] =
            await do_get_usages_from_device(GET_OUR_USAGES, our_usage_count);
        extra_usages['source'] =
            await do_get_usages_from_device(GET_THEIR_USAGES, their_usage_count);
    } catch (e) {
        display_error(e);
    }
}

function set_config_ui_state() {
    document.getElementById('partial_scroll_timeout_input').value = Math.round(config['partial_scroll_timeout'] / 1000);
    document.getElementById('tap_hold_threshold_input').value = Math.round(config['tap_hold_threshold'] / 1000);
    document.getElementById('gpio_debounce_time_input').value = config['gpio_debounce_time_ms'];
    for (let i = 0; i < NLAYERS; i++) {
        document.getElementById('unmapped_passthrough_checkbox' + i).checked =
            config['unmapped_passthrough_layers'].includes(i);
    }
    document.getElementById('interval_override_dropdown').value = config['interval_override'];
    document.getElementById('our_descriptor_number_dropdown').value = config['our_descriptor_number'];
    document.getElementById('ignore_auth_dev_inputs_checkbox').checked = config['ignore_auth_dev_inputs'];
}

function set_mappings_ui_state() {
    clear_children(document.getElementById('mappings'));
    for (const mapping of config['mappings']) {
        add_mapping(mapping);
    }
}

function set_macros_ui_state() {
    let macro_i = 0;
    for (const macro of config['macros']) {
        if (macro_i >= NMACROS) {
            break;
        }

        const macro_element = document.getElementById('macro_' + macro_i);
        const macro_entries = macro_element.querySelector('.macro_entries');
        clear_children(macro_entries);
        for (const entry of macro) {
            const entry_element = add_macro_entry(macro_element);
            for (const item of entry) {
                const item_element = add_macro_item(entry_element);
                const usage_button_element = item_element.querySelector('.macro_item_usage');
                usage_button_element.innerText = readable_target_usage_name(item);
                usage_button_element.title = item;
                usage_button_element.setAttribute('data-hid-usage', item);
            }
            if (entry.length == 0) {
                add_macro_item(entry_element);
            }
        }

        macro_i++;
    }
    set_macro_previews();
}

function set_macro_previews() {
    for (let i = 0; i < NMACROS; i++) {
        const macro_element = document.getElementById('macro_' + i);
        const preview = Array.from(macro_element.querySelectorAll('.macro_entry'),
            (entry_element) => Array.from(entry_element.querySelectorAll('.macro_item_usage'),
                (item_element) => item_element.innerText == "Nothing" ? "∅" : item_element.innerText
            ).join('+')).join(', ');
        macro_element.querySelector('.macro_preview').innerText = preview;
    }
}

function set_expressions_ui_state() {
    const json_to_ui = function (op) {
        if (op.toLowerCase().startsWith('0x')) {
            return op;
        }
        if (/^[0-9-]/.test(op)) {
            return (parseInt(op, 10) / 1000).toString();
        }
        return op;
    }

    let expr_i = 0;
    for (const expr of config['expressions']) {
        if (expr_i >= NEXPRESSIONS) {
            break;
        }

        document.getElementById('expression_' + expr_i).querySelector('.expression_input').value =
            expr.split(/\s+/).map(json_to_ui).join(' ');

        expr_i++;
    }
}

function set_ui_state() {
    if (config['version'] == 3) {
        config['unmapped_passthrough_layers'] = config['unmapped_passthrough'] ? [0] : [];
        delete config['unmapped_passthrough'];
        for (const mapping of config['mappings']) {
            mapping['layers'] = [mapping['layer']];
            delete mapping['layer'];
        }
        config['macros'] = [[], [], [], [], [], [], [], []];
    }
    if (config['version'] < 5) {
        for (const mapping of config['mappings']) {
            mapping['tap'] = false;
            mapping['hold'] = false;
        }
        config['tap_hold_threshold'] = DEFAULT_TAP_HOLD_THRESHOLD;
    }
    if (config['version'] < 6) {
        config['expressions'] = ['', '', '', '', '', '', '', ''];
    }
    if (config['version'] < 7) {
        while (config['macros'].length < NMACROS) {
            config['macros'].push([]);
        }
        config['gpio_debounce_time_ms'] = DEFAULT_GPIO_DEBOUNCE_TIME;
    }
    if (config['version'] < 9) {
        config['our_descriptor_number'] = 0;
        config['ignore_auth_dev_inputs'] = false;
    }
    if (config['version'] < CONFIG_VERSION) {
        config['version'] = CONFIG_VERSION;
    }

    set_config_ui_state();
    set_mappings_ui_state();
    set_macros_ui_state();
    set_expressions_ui_state();
    setup_usages_modals();
}

function add_mapping(mapping) {
    const template = document.getElementById("mapping_template");
    const container = document.getElementById("mappings");
    const clone = template.content.cloneNode(true).firstElementChild;
    clone.querySelector(".delete_button").addEventListener("click", delete_mapping(mapping, clone));
    const sticky_checkbox = clone.querySelector(".sticky_checkbox");
    sticky_checkbox.checked = mapping['sticky'];
    sticky_checkbox.addEventListener("change", sticky_onclick(mapping, sticky_checkbox));
    const tap_checkbox = clone.querySelector(".tap_checkbox");
    tap_checkbox.checked = mapping['tap'];
    tap_checkbox.addEventListener("change", tap_onclick(mapping, tap_checkbox));
    const hold_checkbox = clone.querySelector(".hold_checkbox");
    hold_checkbox.checked = mapping['hold'];
    hold_checkbox.addEventListener("change", hold_onclick(mapping, hold_checkbox));
    const scaling_input = clone.querySelector(".scaling_input");
    scaling_input.value = mapping['scaling'] / 1000;
    scaling_input.addEventListener("input", scaling_onchange(mapping, scaling_input));
    for (let i = 0; i < NLAYERS; i++) {
        const layer_checkbox = clone.querySelector(".layer_checkbox" + i);
        layer_checkbox.checked = mapping['layers'].includes(i);
        layer_checkbox.addEventListener("change", layer_checkbox_onchange(mapping, layer_checkbox, i));
    }
    const source_button = clone.querySelector(".source_button");
    source_button.innerText = readable_usage_name(mapping['source_usage']);
    source_button.setAttribute('data-hid-usage', mapping['source_usage']);
    source_button.title = mapping['source_usage'];
    source_button.addEventListener("click", show_usage_modal(mapping, 'source', source_button));
    const target_button = clone.querySelector(".target_button");
    target_button.innerText = readable_target_usage_name(mapping['target_usage']);
    target_button.setAttribute('data-hid-usage', mapping['target_usage']);
    target_button.title = mapping['target_usage'];
    target_button.addEventListener("click", show_usage_modal(mapping, 'target', target_button));
    container.appendChild(clone);
    set_forced_layers(mapping, clone);
    set_forced_flags(mapping, clone);
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

async function flash_firmware() {
    display_error("HID Remapper should now be in firmware flashing mode. Copy UF2 file to the drive that appeared. If you don't want to flash new firmware at this time, just unplug and replug the device.");
    await send_feature_command(RESET_INTO_BOOTSEL);
}

async function flash_b_side() {
    display_error("Side B should now be flashed with firmware version matching side A. Disconnect and reconnect the device.");
    await send_feature_command(FLASH_B_SIDE);
}

async function pair_new_device() {
    await send_feature_command(PAIR_NEW_DEVICE);
}

async function clear_bonds() {
    await send_feature_command(CLEAR_BONDS);
}

function file_uploaded() {
    const reader = new FileReader();
    reader.onload = function (e) {
        try {
            const new_config = JSON.parse(e.target.result);
            check_json_version(new_config['version']);
            config = new_config;
            set_ui_state();
            validate_ui_expressions();
            switch_to_mappings_tab();
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

async function send_feature_command(command, fields = [], version = CONFIG_VERSION) {
    let buffer = new ArrayBuffer(CONFIG_SIZE);
    let dataview = new DataView(buffer);
    dataview.setUint8(0, version);
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

function display_error_html(message) {
    document.getElementById("error").innerHTML = message;
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

function check_json_version(config_version) {
    if (!([3, 4, 5, 6, 7, 8, 9].includes(config_version))) {
        throw new Error("Incompatible version.");
    }
}

function check_received_version(config_version) {
    if (config_version != CONFIG_VERSION) {
        throw new Error("Incompatible version.");
    }
}

async function check_device_version() {
    // For versions <=3, this isn't a reliable way of checking the config version of the
    // device because it could be version X, ignore our GET_CONFIG call with version Y and
    // just happen to have Y at the right place in the buffer from some previous call done
    // by some other software.
    for (const version of [CONFIG_VERSION, 8, 7, 6, 5, 4, 3, 2]) {
        await send_feature_command(GET_CONFIG, [], version);
        const [received_version] = await read_config_feature([UINT8]);
        if (received_version == version) {
            if (version == CONFIG_VERSION) {
                return true;
            }
            display_error_html('<p>Incompatible version (' + version + ').</p>' +
                '<p>Please consider upgrading your HID Remapper firmware to the <a href="https://github.com/jfedor2/hid-remapper/releases/latest">latest version</a>.</p>' +
                '<p class="mb-0">Alternatively, click <a href="v' + version + '/">here</a> for an older version of the configuration tool that should be compatible with your device.</p>');
            return false;
        }
    }

    display_error_html('<p>Incompatible version.</p><p class="mb-0">Please consider upgrading your HID Remapper firmware to the <a href="https://github.com/jfedor2/hid-remapper/releases/latest">latest version</a>.</p>');
    return false;
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
        set_forced_layers(mapping, element.closest(".mapping_container"));
    };
}

function tap_onclick(mapping, element) {
    return function () {
        mapping['tap'] = element.checked;
    };
}

function hold_onclick(mapping, element) {
    return function () {
        mapping['hold'] = element.checked;
    };
}

function scaling_onchange(mapping, element) {
    return function () {
        mapping['scaling'] = element.value === '' ? DEFAULT_SCALING : Math.round(parseFloat(element.value) * 1000);
    };
}

function layer_checkbox_onchange(mapping, element, layer) {
    return function () {
        if (element.checked) {
            if (!mapping['layers'].includes(layer)) {
                mapping['layers'].push(layer)
                mapping['layers'].sort();
            }
        } else {
            mapping['layers'] = mapping['layers'].filter((x) => x != layer);
        }
    };
}

function expression_onchange(i) {
    const ui_to_json = function (op) {
        if (op.toLowerCase().startsWith('0x')) {
            if (isNaN(parseInt(op, 16))) {
                throw new Error('Invalid expression: "' + op + '"');
            }
            return op;
        }
        if (/^[0-9-]/.test(op)) {
            const x = parseFloat(op);
            if (isNaN(x)) {
                throw new Error('Invalid expression: "' + op + '"');
            }
            return Math.round(x * 1000).toString();
        }
        if ((op.toUpperCase() in ops) && !['PUSH', 'PUSH_USAGE'].includes(op.toUpperCase())) {
            return op.toLowerCase();
        }
        throw new Error('Invalid expression: "' + op + '"');
    }

    return function () {
        const expr_input = document.getElementById('expression_' + i).querySelector('.expression_input');
        try {
            config['expressions'][i] =
                expr_input.value.split(/\s+/).filter((x) => (x.length > 0)).map(ui_to_json).join(' ');
            expr_input.classList.remove('is-invalid');
        } catch (e) {
            expr_input.classList.add('is-invalid');
            config['expressions'][i] = '';
        }
    }
}

function show_usage_modal(mapping, source_or_target, element) {
    return function () {
        // XXX it would be better not to do this every time we show the modal
        const modal_element = document.getElementById(
            (source_or_target == 'source' ? 'source' : 'target') + '_usage_modal');
        const modal_ = source_or_target == 'source' ? source_modal : target_modal;
        modal_element.querySelectorAll('.usage_button').forEach((button) => {
            let clone = button.cloneNode(true);
            button.parentNode.replaceChild(clone, button); // to clear existing event listeners
            clone.addEventListener("click", function () {
                let usage = clone.getAttribute('data-hid-usage');
                if (mapping !== null) {
                    mapping[source_or_target + '_usage'] = usage;
                }
                element.innerText =
                    source_or_target == "source" ? readable_usage_name(usage) : readable_target_usage_name(usage);

                element.title = usage;

                if (source_or_target == "target") {
                    set_forced_layers(mapping, element.closest(".mapping_container"));
                }

                if (source_or_target == "source") {
                    set_forced_flags(mapping, element.closest(".mapping_container"));
                }

                if (source_or_target == "macro_item") {
                    element.setAttribute('data-hid-usage', usage);
                    set_macros_config_from_ui_state();
                }

                modal_.hide();
            });
        });
        modal_.show();
    };
}

function add_mapping_onclick() {
    let new_mapping = {
        'source_usage': '0x00000000',
        'target_usage': '0x00000000',
        'layers': [0],
        'sticky': false,
        'tap': false,
        'hold': false,
        'scaling': DEFAULT_SCALING
    };
    config['mappings'].push(new_mapping);
    add_mapping(new_mapping);
}

function setup_usages_modals() {
    setup_usage_modal('source');
    setup_usage_modal('target');
}

function setup_usage_modal(source_or_target) {
    const modal_element = document.getElementById(source_or_target + '_usage_modal');
    let usage_classes = {
        'mouse': modal_element.querySelector('.mouse_usages'),
        'keyboard': modal_element.querySelector('.keyboard_usages'),
        'media': modal_element.querySelector('.media_usages'),
        'other': modal_element.querySelector('.other_usages'),
        'extra': modal_element.querySelector('.extra_usages'),
    };
    for (const [usage_class, element] of Object.entries(usage_classes)) {
        clear_children(element);
    }
    let template = document.getElementById('usage_button_template');
    const relevant_usages = usages[source_or_target == 'source' ? 'source' : config['our_descriptor_number']];
    for (const [usage, usage_def] of Object.entries(relevant_usages)) {
        let clone = template.content.cloneNode(true).firstElementChild;
        clone.innerText = usage_def['name'];
        clone.title = usage;
        clone.setAttribute('data-hid-usage', usage);
        usage_classes[usage_def['class']].appendChild(clone);
    }
    for (const usage_ of extra_usages[source_or_target]) {
        if (!(usage_ in relevant_usages)) {
            let clone = template.content.cloneNode(true).firstElementChild;
            clone.innerText = usage_;
            clone.title = usage_;
            clone.setAttribute('data-hid-usage', usage_);
            usage_classes['extra'].appendChild(clone);
        }
    }
}

function setup_macros() {
    const macros_container = document.getElementById('macros_container');
    let template = document.getElementById('macro_template');
    for (let i = 0; i < NMACROS; i++) {
        let clone = template.content.cloneNode(true).firstElementChild;
        clone.id = 'macro_' + i;
        clone.querySelector('.accordion-button').setAttribute('data-bs-target', '#collapse_' + i);
        clone.querySelector('.accordion-button').querySelector('.macro_name').innerText = 'Macro ' + (i + 1);
        clone.querySelector('.accordion-collapse').id = 'collapse_' + i;
        clone.querySelector('.add_macro_entry').addEventListener("click", () => {
            const entry_element = add_macro_entry(clone);
            add_macro_item(entry_element);
            set_macros_config_from_ui_state();
        });
        macros_container.appendChild(clone);
    }
}

function add_macro_entry(element) {
    const template = document.getElementById('macro_entry_template');
    const clone = template.content.cloneNode(true).firstElementChild;
    clone.querySelector('.add_macro_item').addEventListener("click", () => {
        add_macro_item(clone);
        set_macros_config_from_ui_state();
    });
    element.querySelector('.macro_entries').appendChild(clone);
    return clone;
}

function add_macro_item(element) {
    const template = document.getElementById('macro_item_template');
    const clone = template.content.cloneNode(true).firstElementChild;
    const button = clone.querySelector('.macro_item_usage');
    button.addEventListener("click", show_usage_modal(null, 'macro_item', button));
    clone.querySelector('.delete_macro_item').addEventListener("click", () => {
        delete_macro_item(clone);
        set_macros_config_from_ui_state();
    });
    element.insertBefore(clone, element.querySelector(".add_item_container"));
    return clone;
}

function delete_macro_item(element) {
    const parent = element.parentNode;
    parent.removeChild(element);
    if (parent.querySelectorAll('.macro_item').length == 0) {
        const macro_entry = parent.closest('.macro_entry');
        macro_entry.parentNode.removeChild(macro_entry);
    }
}

function set_macros_config_from_ui_state() {
    config['macros'] = [];
    for (let i = 0; i < NMACROS; i++) {
        let macro = [];
        const macro_element = document.querySelector('#macro_' + i);
        for (const entry_element of macro_element.querySelectorAll('.macro_entry')) {
            let entry = [];
            for (const item_element of entry_element.querySelectorAll('.macro_item_usage')) {
                const usage = item_element.getAttribute('data-hid-usage');
                if (usage != '0x00000000') {
                    entry.push(usage);
                }
            }
            macro.push(entry);
        }
        config['macros'].push(macro);
    }

    set_macro_previews();
}

function setup_expressions() {
    const expr_container = document.getElementById('expressions_container');
    let template = document.getElementById('expression_template');
    for (let i = 0; i < NEXPRESSIONS; i++) {
        let clone = template.content.cloneNode(true).firstElementChild;
        clone.id = 'expression_' + i;
        clone.querySelector('.expression_label').innerText = 'Expression ' + (i + 1);
        clone.querySelector('.expression_input').addEventListener("input", expression_onchange(i));
        expr_container.appendChild(clone);
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

function tap_hold_threshold_onchange() {
    let value = document.getElementById('tap_hold_threshold_input').value;
    if (value === '') {
        value = DEFAULT_TAP_HOLD_THRESHOLD;
    } else {
        value = Math.round(value * 1000);
    }
    config['tap_hold_threshold'] = value;
}

function gpio_debounce_time_onchange() {
    let value = parseInt(document.getElementById('gpio_debounce_time_input').value, 10);
    if (isNaN(value)) {
        value = DEFAULT_GPIO_DEBOUNCE_TIME;
    }
    config['gpio_debounce_time_ms'] = value;
}

function unmapped_passthrough_onchange() {
    config['unmapped_passthrough_layers'] = [];
    for (let i = 0; i < NLAYERS; i++) {
        if (document.getElementById("unmapped_passthrough_checkbox" + i).checked) {
            config['unmapped_passthrough_layers'].push(i);
        }
    }
}

function interval_override_onchange() {
    config['interval_override'] = parseInt(document.getElementById("interval_override_dropdown").value, 10);
}

function our_descriptor_number_onchange() {
    config['our_descriptor_number'] = parseInt(document.getElementById("our_descriptor_number_dropdown").value, 10);
    set_ui_state();
}

function ignore_auth_dev_inputs_onchange() {
    config['ignore_auth_dev_inputs'] = document.getElementById("ignore_auth_dev_inputs_checkbox").checked;
}

function load_example(n) {
    config = structuredClone(examples[n]['config']);
    set_ui_state();
    validate_ui_expressions();
    switch_to_mappings_tab();
}

function setup_examples() {
    const element = document.getElementById("examples");
    const template = document.getElementById("example_template");
    for (let i = 0; i < examples.length; i++) {
        const clone = template.content.cloneNode(true).firstElementChild;
        const link = clone.querySelector('a');
        link.innerText = examples[i]['description'];
        link.addEventListener("click", () => load_example(i));
        element.appendChild(clone);
    }
}

function hid_on_disconnect(event) {
    if (event.device === device) {
        device = null;
        device_buttons_set_disabled_state(true);
    }
}

function device_buttons_set_disabled_state(state) {
    document.getElementById("load_from_device").disabled = state;
    document.getElementById("save_to_device").disabled = state;
    document.getElementById("flash_firmware").disabled = state;
    document.getElementById("flash_b_side").disabled = state;
    document.getElementById("pair_new_device").disabled = state;
    document.getElementById("clear_bonds").disabled = state;
}

function bluetooth_buttons_set_visibility(visible) {
    document.getElementById("pair_new_device_container").classList.toggle("d-none", !visible);
    document.getElementById("clear_bonds_container").classList.toggle("d-none", !visible);
    document.getElementById("flash_b_side_container").classList.toggle("d-none", visible);
}

function mask_to_layer_list(layer_mask) {
    let layers = [];
    for (let i = 0; i < NLAYERS; i++) {
        if ((layer_mask & (1 << i)) != 0) {
            layers.push(i);
        }
    }
    return layers;
}

function layer_list_to_mask(layers) {
    let layer_mask = 0;

    for (const layer of layers) {
        layer_mask |= (1 << layer);
    }

    return layer_mask;
}

function readable_usage_name(usage, default_to_hex = true) {
    if (((usage & 0xFFFF0000) >>> 0) == MIDI_USAGE_PAGE) {
        const status = (usage >> 8) & 0xF0;
        const channel = (usage >> 8) & 0x0F;
        const prefix = 'MIDI CH' + channel;
        switch (status) {
            case 0x90:
                return prefix + ' note ' + (usage & 0xFF);
            case 0xA0:
                return prefix + ' PKP ' + (usage & 0xFF);
            case 0xB0:
                return prefix + ' CC' + (usage & 0xFF);
            case 0xC0:
                return prefix + ' program';
            case 0xD0:
                return prefix + ' CP';
            case 0xE0:
                return prefix + ' PB';
        }
    }
    return (usage in usages['source']) ? usages['source'][usage]['name'] : (default_to_hex ? usage : '');
}

function readable_target_usage_name(usage) {
    return (usage in usages[config['our_descriptor_number']]) ? usages[config['our_descriptor_number']][usage]['name'] : usage;
}

function set_forced_layers(mapping, mapping_container) {
    for (let i = 0; i < NLAYERS; i++) {
        mapping_container.querySelector(".layer_checkbox" + i).disabled = false;
    }
    const usage_int = parseInt(mapping['target_usage'], 16);
    if (((usage_int & 0xFFFF0000) >>> 0) == LAYERS_USAGE_PAGE) {
        const layer = usage_int & 0xFFFF;
        const layer_checkbox = mapping_container.querySelector(".layer_checkbox" + layer);
        if (mapping['sticky']) {
            mapping['layers'] = mapping['layers'].filter((x) => x != layer);
            layer_checkbox.checked = false;
        } else {
            if (!mapping['layers'].includes(layer)) {
                mapping['layers'].push(layer)
                mapping['layers'].sort();
            }
            layer_checkbox.checked = true;
        }
        layer_checkbox.disabled = true;
    }
}

function set_forced_flags(mapping, mapping_container) {
    mapping_container.querySelector(".sticky_checkbox").disabled = false;
    mapping_container.querySelector(".tap_checkbox").disabled = false;
    mapping_container.querySelector(".hold_checkbox").disabled = false;
    const usage_int = parseInt(mapping['source_usage'], 16);
    if (((usage_int & 0xFFFF0000) >>> 0) == EXPR_USAGE_PAGE) {
        mapping_container.querySelector(".sticky_checkbox").checked = false;
        mapping_container.querySelector(".tap_checkbox").checked = false;
        mapping_container.querySelector(".hold_checkbox").checked = false;
        mapping_container.querySelector(".sticky_checkbox").disabled = true;
        mapping_container.querySelector(".tap_checkbox").disabled = true;
        mapping_container.querySelector(".hold_checkbox").disabled = true;
        mapping['sticky'] = false;
        mapping['tap'] = false;
        mapping['hold'] = false;
    }
}

function switch_to_mappings_tab() {
    bootstrap.Tab.getOrCreateInstance(document.getElementById("nav-mappings-tab")).show();
}

function expr_to_elems(expr) {
    const convert_elem = function (elem) {
        if (elem.toLowerCase().startsWith('0x')) {
            return [ops["PUSH_USAGE"], parseInt(elem, 16)];
        }
        if (/^[0-9-]/.test(elem)) {
            return [ops["PUSH"], parseInt(elem, 10)];
        }
        if (elem.toUpperCase() in ops) {
            return [ops[elem.toUpperCase()]];
        }
        throw new Error('Invalid expression: "' + elem + '"');
    }

    return expr.split(/\s+/).filter((x) => (x.length > 0)).map(convert_elem);
}

function validate_ui_expressions() {
    for (let i = 0; i < NEXPRESSIONS; i++) {
        expression_onchange(i)();
    }
}

function input_report_received(event) {
    if (event.reportId == REPORT_ID_MONITOR) {
        document.querySelectorAll('.monitor_row').forEach((element) => {
            element.classList.remove('bg-light');
        });
        for (let i = 0; i < 7; i++) {
            const usage = "0x" + event.data.getUint32(i * 8, true).toString(16).padStart(8, "0");
            const value = event.data.getInt32(i * 8 + 4, true);
            if (usage != 0) {
                update_monitor_ui(usage, value);
            }
        }
    }
}

function update_monitor_ui(usage, value) {
    let element = document.getElementById('monitor_usage_' + usage);
    if (element == null) {
        const template = document.getElementById("monitor_template");
        const container = document.getElementById("monitor_container");
        element = template.content.cloneNode(true).firstElementChild;
        element.querySelector('.monitor_usage').innerText = usage;
        element.querySelector('.monitor_readable_name').innerText = readable_usage_name(usage, false);
        element.id = 'monitor_usage_' + usage;
        container.appendChild(element);
    }

    document.getElementById('monitor_header').classList.remove('d-none');

    element.querySelector('.monitor_last_value').innerText = value;
    element.classList.add('bg-light');
    if (!(usage in monitor_min_val) || (value < monitor_min_val[usage])) {
        monitor_min_val[usage] = value;
        element.querySelector('.monitor_min_value').innerText = value;
    }
    if (!(usage in monitor_max_val) || (value > monitor_max_val[usage])) {
        monitor_max_val[usage] = value;
        element.querySelector('.monitor_max_value').innerText = value;
    }
}

function monitor_clear() {
    clear_children(document.getElementById("monitor_container"));
    document.getElementById('monitor_header').classList.add('d-none');
    monitor_min_val = {};
    monitor_max_val = {};
}

async function set_monitor_enabled(enabled) {
    monitor_enabled = enabled;
    if (device != null) {
        await send_feature_command(SET_MONITOR_ENABLED, [[UINT8, monitor_enabled ? 1 : 0]]);
    }
}

async function monitor_tab_shown(event) {
    await set_monitor_enabled(true);
}

async function monitor_tab_hide(event) {
    await set_monitor_enabled(false);
}
