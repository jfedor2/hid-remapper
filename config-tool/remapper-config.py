#!/usr/bin/env python3

import os
import struct
import binascii
import traceback
import gi
import hid


gi.require_version("Gtk", "3.0")
from gi.repository import Gtk

VID = 0xCAFE
PID = 0xBAF1
CONFIG_SIZE = 30
REPORT_ID = 3
CONFIG_VERSION = 1
NBUTTONS = 8
NAXES = 4

AXIS_FUNCTIONS = (
    ("None", "0"),
    ("Cursor X", "1"),
    ("Cursor Y", "2"),
    ("V scroll", "3"),
    ("H scroll", "4"),
    ("Cursor X (inverted)", "-1"),
    ("Cursor Y (inverted)", "-2"),
    ("V scroll (inverted)", "-3"),
    ("H scroll (inverted)", "-4"),
)

AXIS_NAMES = (
    "X axis",
    "Y axis",
    "V scroll",
    "H scroll",
)

BUTTON_FUNCTIONS = (
    ("None", "0"),
    ("Button 1 (left)", "1"),
    ("Button 2 (right)", "2"),
    ("Button 3 (middle)", "3"),
    ("Button 4 (back)", "4"),
    ("Button 5 (forward)", "5"),
    ("Button 6", "6"),
    ("Button 7", "7"),
    ("Button 8", "8"),
    ("Click-drag", "9"),
    ("Shift", "10"),
)


def make_model(options):
    model = Gtk.ListStore(str, str)
    for o in options:
        model.append(o)
    return model


def make_dropdown(model):
    dropdown = Gtk.ComboBox.new_with_model(model)
    renderer_text = Gtk.CellRendererText()
    dropdown.pack_start(renderer_text, True)
    dropdown.add_attribute(renderer_text, "text", 0)
    dropdown.set_id_column(1)
    dropdown.set_active_id("0")
    return dropdown


def make_scale():
    scale = Gtk.Scale.new_with_range(Gtk.Orientation.HORIZONTAL, 1, 120, 1)
    scale.connect("format-value", lambda _, value: str(int(value) * 100))
    return scale


class RemapperConfigWindow(Gtk.Window):
    def __init__(self):
        axis_function_model = make_model(AXIS_FUNCTIONS)
        button_function_model = make_model(BUTTON_FUNCTIONS)
        self.devices_model = Gtk.ListStore(str, str)

        Gtk.Window.__init__(self, title="Remapper Configuration")

        self.set_border_width(10)

        vbox = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=6)

        devices_hbox = Gtk.Box()
        self.devices_dropdown = make_dropdown(self.devices_model)
        devices_hbox.pack_start(self.devices_dropdown, True, True, 0)
        refresh_button = Gtk.Button.new_from_icon_name(
            "view-refresh", Gtk.IconSize.BUTTON
        )
        refresh_button.set_tooltip_text("Refresh device list")
        refresh_button.connect("clicked", self.refresh_button_clicked)
        devices_hbox.pack_start(refresh_button, False, False, 0)
        vbox.pack_start(devices_hbox, True, True, 0)

        actions_hbox = Gtk.Box()
        self.load_button = Gtk.Button.new_with_label("Load from device")
        self.load_button.connect("clicked", self.load_button_clicked)
        actions_hbox.pack_start(self.load_button, True, True, 0)
        self.save_button = Gtk.Button.new_with_label("Save to device")
        self.save_button.connect("clicked", self.save_button_clicked)
        actions_hbox.pack_start(self.save_button, True, True, 0)
        vbox.pack_start(actions_hbox, True, True, 0)

        grid = Gtk.Grid(column_spacing=6, row_spacing=6)

        row = 0
        grid.attach(Gtk.Label("Normal", halign=Gtk.Align.CENTER), 1, row, 1, 1)
        grid.attach(Gtk.Label("Shifted", halign=Gtk.Align.CENTER), 2, row, 1, 1)

        self.axis_dropdowns = []
        self.axis_shifted_dropdowns = []
        for i in range(NAXES):
            row += 1
            grid.attach(Gtk.Label(AXIS_NAMES[i], halign=Gtk.Align.END), 0, row, 1, 1)
            axis_dropdown = make_dropdown(axis_function_model)
            grid.attach(axis_dropdown, 1, row, 1, 1)
            self.axis_dropdowns.append(axis_dropdown)
            axis_shifted_dropdown = make_dropdown(axis_function_model)
            grid.attach(axis_shifted_dropdown, 2, row, 1, 1)
            self.axis_shifted_dropdowns.append(axis_shifted_dropdown)

        self.button_dropdowns = []
        self.button_shifted_dropdowns = []
        for i in range(NBUTTONS):
            row += 1
            grid.attach(Gtk.Label("Button {}".format(i+1), halign=Gtk.Align.END), 0, row, 1, 1)
            button_dropdown = make_dropdown(button_function_model)
            grid.attach(button_dropdown, 1, row, 1, 1)
            self.button_dropdowns.append(button_dropdown)
            button_shifted_dropdown = make_dropdown(button_function_model)
            grid.attach(button_shifted_dropdown, 2, row, 1, 1)
            self.button_shifted_dropdowns.append(button_shifted_dropdown)

        vbox.pack_start(grid, True, True, 0)

        self.refresh_device_list()

        self.add(vbox)

    def wrap_exception_in_dialog(self, f):
        try:
            f()
        except Exception as e:
            dialog = Gtk.MessageDialog(
                transient_for=self,
                flags=0,
                message_type=Gtk.MessageType.ERROR,
                buttons=Gtk.ButtonsType.OK,
                text=traceback.format_exc(),
            )
            dialog.run()
            dialog.destroy()

    def refresh_button_clicked(self, button):
        self.refresh_device_list()

    def refresh_device_list(self):
        self.devices_model.clear()
        devices = [
            d
            for d in hid.enumerate()
            if d["vendor_id"] == VID and d["product_id"] == PID
        ]
        if devices:
            for d in devices:
                self.devices_model.append(
                    (
                        f"{d['manufacturer_string']} {d['product_string']}",
                        str(d["path"], "ascii"),
                    )
                )
            self.load_button.set_sensitive(True)
            self.save_button.set_sensitive(True)
        else:
            self.devices_model.append(("No devices found", "NULL"))
            self.load_button.set_sensitive(False)
            self.save_button.set_sensitive(False)
        self.devices_dropdown.set_active(0)

    def load_button_clicked(self, button):
        self.wrap_exception_in_dialog(self.load_config_from_device)

    def load_config_from_device(self):
        path = self.devices_dropdown.get_active_id()
        device = hid.Device(path=bytes(path, "ascii"))
        data = device.get_feature_report(REPORT_ID, CONFIG_SIZE + 1)
        (
            report_id,
            version,
            command,
            *axes_and_buttons,
            crc32,
        ) = struct.unpack("<BBb4b4b8b8bL", data)
        if version != CONFIG_VERSION:
            raise Exception("Incompatible config version received from device.")
        axes = axes_and_buttons[0:NAXES]
        axes_shifted = axes_and_buttons[NAXES:2*NAXES]
        buttons = axes_and_buttons[2*NAXES:2*NAXES+NBUTTONS]
        buttons_shifted = axes_and_buttons[2*NAXES+NBUTTONS:2*NAXES+2*NBUTTONS]
        for i in range(NAXES):
            self.axis_dropdowns[i].set_active_id(str(axes[i]))
            self.axis_shifted_dropdowns[i].set_active_id(str(axes_shifted[i]))
        for i in range(NBUTTONS):
            self.button_dropdowns[i].set_active_id(str(buttons[i]))
            self.button_shifted_dropdowns[i].set_active_id(str(buttons_shifted[i]))

    def save_button_clicked(self, button):
        self.wrap_exception_in_dialog(self.save_config_to_device)

    def save_config_to_device(self):
        command = 0
        axes = [ int(x.get_active_id()) for x in self.axis_dropdowns ]
        axes_shifted = [ int(x.get_active_id()) for x in self.axis_shifted_dropdowns ]
        buttons = [ int(x.get_active_id()) for x in self.button_dropdowns ]
        buttons_shifted = [ int(x.get_active_id()) for x in self.button_shifted_dropdowns ]

        data = struct.pack(
            "<BBb4b4b8b8b",
            REPORT_ID,
            CONFIG_VERSION,
            command,
            *axes,
            *axes_shifted,
            *buttons,
            *buttons_shifted,
        )
        crc32 = binascii.crc32(data[1:])
        crc_bytes = struct.pack("<L", crc32)
        data += crc_bytes

        path = self.devices_dropdown.get_active_id()
        device = hid.Device(path=bytes(path, "ascii"))
        device.send_feature_report(data)


def main():
    win = RemapperConfigWindow()
    win.connect("delete-event", Gtk.main_quit)
    win.show_all()
    Gtk.main()


if __name__ == "__main__":
    main()
