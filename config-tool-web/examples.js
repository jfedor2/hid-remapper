const examples = [
    {
        'description': 'map caps lock to control',
        'config': {
            "version": 3,
            "unmapped_passthrough": true,
            "partial_scroll_timeout": 1000000,
            "interval_override": 0,
            "mappings": [
                {
                    "source_usage": "0x00070039",
                    "target_usage": "0x000700e0",
                    "layer": 0,
                    "sticky": false,
                    "scaling": 1000
                }
            ]
        }
    },
    {
        'description': 'swap left/right mouse buttons',
        'config': {
            "version": 3,
            "unmapped_passthrough": true,
            "partial_scroll_timeout": 1000000,
            "interval_override": 0,
            "mappings": [
                {
                    "source_usage": "0x00090001",
                    "target_usage": "0x00090002",
                    "layer": 0,
                    "sticky": false,
                    "scaling": 1000
                },
                {
                    "source_usage": "0x00090002",
                    "target_usage": "0x00090001",
                    "layer": 0,
                    "sticky": false,
                    "scaling": 1000
                }
            ]
        }
    },
    {
        'description': 'middle button is ctrl-c, right button is ctrl-v',
        'config': {
            "version": 3,
            "unmapped_passthrough": true,
            "partial_scroll_timeout": 1000000,
            "interval_override": 0,
            "mappings": [
                {
                    "source_usage": "0x00090003",
                    "target_usage": "0x000700e0",
                    "layer": 0,
                    "sticky": false,
                    "scaling": 1000
                },
                {
                    "source_usage": "0x00090003",
                    "target_usage": "0x00070006",
                    "layer": 0,
                    "sticky": false,
                    "scaling": 1000
                },
                {
                    "source_usage": "0x00090002",
                    "target_usage": "0x000700e0",
                    "layer": 0,
                    "sticky": false,
                    "scaling": 1000
                },
                {
                    "source_usage": "0x00090002",
                    "target_usage": "0x00070019",
                    "layer": 0,
                    "sticky": false,
                    "scaling": 1000
                }
            ]
        }
    },
    {
        'description': 'invert scroll wheel direction',
        'config': {
            "version": 3,
            "unmapped_passthrough": true,
            "partial_scroll_timeout": 1000000,
            "interval_override": 0,
            "mappings": [
                {
                    "source_usage": "0x00010038",
                    "target_usage": "0x00010038",
                    "layer": 0,
                    "sticky": false,
                    "scaling": -1000
                }
            ]
        }
    },
    {
        'description': 'moving the mouse scrolls when middle button held',
        'config': {
            "version": 3,
            "unmapped_passthrough": true,
            "partial_scroll_timeout": 1000000,
            "interval_override": 0,
            "mappings": [
                {
                    "source_usage": "0x00090003",
                    "target_usage": "0xfff10001",
                    "layer": 0,
                    "sticky": false,
                    "scaling": 1000
                },
                {
                    "source_usage": "0x00010031",
                    "target_usage": "0x00010038",
                    "layer": 1,
                    "sticky": false,
                    "scaling": 10
                },
                {
                    "source_usage": "0x00010030",
                    "target_usage": "0x000c0238",
                    "layer": 1,
                    "sticky": false,
                    "scaling": 10
                }
            ]
        }
    },
    {
        'description': 'mouse precision mode when middle button held',
        'config': {
            "version": 3,
            "unmapped_passthrough": true,
            "partial_scroll_timeout": 1000000,
            "interval_override": 0,
            "mappings": [
                {
                    "source_usage": "0x00090003",
                    "target_usage": "0xfff10001",
                    "layer": 0,
                    "sticky": false,
                    "scaling": 1000
                },
                {
                    "source_usage": "0x00010030",
                    "target_usage": "0x00010030",
                    "layer": 1,
                    "sticky": false,
                    "scaling": 250
                },
                {
                    "source_usage": "0x00010031",
                    "target_usage": "0x00010031",
                    "layer": 1,
                    "sticky": false,
                    "scaling": 250
                }
            ]
        }
    },
    {
        'description': 'rotate mouse by 90 degrees',
        'config': {
            "version": 3,
            "unmapped_passthrough": true,
            "partial_scroll_timeout": 1000000,
            "interval_override": 0,
            "mappings": [
                {
                    "source_usage": "0x00010030",
                    "target_usage": "0x00010031",
                    "layer": 0,
                    "sticky": false,
                    "scaling": 1000
                },
                {
                    "source_usage": "0x00010031",
                    "target_usage": "0x00010030",
                    "layer": 0,
                    "sticky": false,
                    "scaling": -1000
                }
            ]
        }
    },
    {
        'description': 'rotate mouse by 30 degrees',
        'config': {
            "version": 3,
            "unmapped_passthrough": true,
            "partial_scroll_timeout": 1000000,
            "interval_override": 0,
            "mappings": [
                {
                    "source_usage": "0x00010030",
                    "target_usage": "0x00010030",
                    "layer": 0,
                    "sticky": false,
                    "scaling": 866
                },
                {
                    "source_usage": "0x00010030",
                    "target_usage": "0x00010031",
                    "layer": 0,
                    "sticky": false,
                    "scaling": -500
                },
                {
                    "source_usage": "0x00010031",
                    "target_usage": "0x00010030",
                    "layer": 0,
                    "sticky": false,
                    "scaling": 500
                },
                {
                    "source_usage": "0x00010031",
                    "target_usage": "0x00010031",
                    "layer": 0,
                    "sticky": false,
                    "scaling": 866
                }
            ]
        }
    },
    {
        'description': 'arrows act as mouse when caps lock held',
        'config': {
            "version": 3,
            "unmapped_passthrough": true,
            "partial_scroll_timeout": 1000000,
            "interval_override": 0,
            "mappings": [
                {
                    "source_usage": "0x00070039",
                    "target_usage": "0xfff10001",
                    "layer": 0,
                    "sticky": false,
                    "scaling": 1000
                },
                {
                    "source_usage": "0x00070050",
                    "target_usage": "0x00010030",
                    "layer": 1,
                    "sticky": false,
                    "scaling": -500
                },
                {
                    "source_usage": "0x0007004f",
                    "target_usage": "0x00010030",
                    "layer": 1,
                    "sticky": false,
                    "scaling": 500
                },
                {
                    "source_usage": "0x00070052",
                    "target_usage": "0x00010031",
                    "layer": 1,
                    "sticky": false,
                    "scaling": -500
                },
                {
                    "source_usage": "0x00070051",
                    "target_usage": "0x00010031",
                    "layer": 1,
                    "sticky": false,
                    "scaling": 500
                },
                {
                    "source_usage": "0x000700e4",
                    "target_usage": "0x00090001",
                    "layer": 1,
                    "sticky": false,
                    "scaling": 1000
                }
            ]
        }
    },
    {
        'description': 'right button is left button with drag-lock',
        'config': {
            "version": 3,
            "unmapped_passthrough": true,
            "partial_scroll_timeout": 1000000,
            "interval_override": 0,
            "mappings": [
                {
                    "source_usage": "0x00090002",
                    "target_usage": "0x00090001",
                    "layer": 0,
                    "sticky": true,
                    "scaling": 1000
                }
            ]
        }
    },
    {
        'description': 'one button drag-lock (simple): long press to engage, long press to disengage',
        'config': {
            "version": 12,
            "unmapped_passthrough_layers": [
                0,
                1,
                2,
                3,
                4,
                5,
                6,
                7
            ],
            "partial_scroll_timeout": 1000000,
            "tap_hold_threshold": 200000,
            "gpio_debounce_time_ms": 5,
            "interval_override": 0,
            "our_descriptor_number": 0,
            "ignore_auth_dev_inputs": false,
            "macro_entry_duration": 1,
            "gpio_output_mode": 0,
            "mappings": [
                {
                    "target_usage": "0x00090001",
                    "source_usage": "0x00090001",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": true,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090001",
                    "source_usage": "0x00090001",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": true,
                    "tap": false,
                    "hold": true,
                    "source_port": 0,
                    "target_port": 0
                }
            ],
            "macros": [
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                []
            ],
            "expressions": [
                "",
                "",
                "",
                "",
                "",
                "",
                "",
                ""
            ],
            "quirks": []
        }
    },
    {
        'description': 'one button drag-lock (advanced): long press to engage, any click to disengage',
        'config': {
            "version": 12,
            "unmapped_passthrough_layers": [
                0,
                1,
                2,
                3,
                4,
                5,
                6,
                7
            ],
            "partial_scroll_timeout": 1000000,
            "tap_hold_threshold": 200000,
            "gpio_debounce_time_ms": 5,
            "interval_override": 0,
            "our_descriptor_number": 0,
            "ignore_auth_dev_inputs": false,
            "macro_entry_duration": 1,
            "gpio_output_mode": 0,
            "mappings": [
                {
                    "target_usage": "0x00090001",
                    "source_usage": "0x00090001",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090001",
                    "source_usage": "0xfff50001",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00000000",
                    "source_usage": "0x00090001",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": true,
                    "source_port": 0,
                    "target_port": 0
                }
            ],
            "macros": [
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                []
            ],
            "expressions": [
                "1000 recall eol eol 2000 recall eol 0x00090001 input_state_binary not eol mul eol bitwise_or eol eol 1000 recall eol 0x00090001 prev_input_state_binary eol mul eol 0x00090001 input_state_binary not eol mul eol not eol mul eol eol 1000 store",
                "0x00090001 hold_state 2000 store",
                "",
                "",
                "",
                "",
                "",
                ""
            ],
            "quirks": []
        }
    },
    {
        'description': 'disable windows keys',
        'config': {
            "version": 3,
            "unmapped_passthrough": true,
            "partial_scroll_timeout": 1000000,
            "interval_override": 0,
            "mappings": [
                {
                    "source_usage": "0x000700e3",
                    "target_usage": "0x00000000",
                    "layer": 0,
                    "sticky": false,
                    "scaling": 1000
                },
                {
                    "source_usage": "0x000700e7",
                    "target_usage": "0x00000000",
                    "layer": 0,
                    "sticky": false,
                    "scaling": 1000
                }
            ]
        }
    },
    {
        'description': 'wheel switches tabs',
        'config': {
            "version": 3,
            "unmapped_passthrough": true,
            "partial_scroll_timeout": 1000000,
            "interval_override": 0,
            "mappings": [
                {
                    "source_usage": "0x00010038",
                    "target_usage": "0x000700e0",
                    "layer": 0,
                    "sticky": false,
                    "scaling": -1000
                },
                {
                    "source_usage": "0x00010038",
                    "target_usage": "0x0007002b",
                    "layer": 0,
                    "sticky": false,
                    "scaling": -1000
                },
                {
                    "source_usage": "0x00010038",
                    "target_usage": "0x000700e0",
                    "layer": 0,
                    "sticky": false,
                    "scaling": 1000
                },
                {
                    "source_usage": "0x00010038",
                    "target_usage": "0x000700e1",
                    "layer": 0,
                    "sticky": false,
                    "scaling": 1000
                },
                {
                    "source_usage": "0x00010038",
                    "target_usage": "0x0007002b",
                    "layer": 0,
                    "sticky": false,
                    "scaling": 1000
                }
            ]
        }
    },
    {
        'description': 'middle button cycles DPI',
        'config': {
            "version": 4,
            "unmapped_passthrough_layers": [
                0,
                1,
                2,
                3
            ],
            "partial_scroll_timeout": 1000000,
            "interval_override": 0,
            "mappings": [
                {
                    "source_usage": "0x00090003",
                    "target_usage": "0xfff10001",
                    "layers": [
                        0
                    ],
                    "sticky": true,
                    "scaling": 1000
                },
                {
                    "source_usage": "0x00090003",
                    "target_usage": "0xfff10002",
                    "layers": [
                        1
                    ],
                    "sticky": true,
                    "scaling": 1000
                },
                {
                    "source_usage": "0x00010030",
                    "target_usage": "0x00010030",
                    "layers": [
                        1
                    ],
                    "sticky": false,
                    "scaling": 500
                },
                {
                    "source_usage": "0x00010031",
                    "target_usage": "0x00010031",
                    "layers": [
                        1
                    ],
                    "sticky": false,
                    "scaling": 500
                },
                {
                    "source_usage": "0x00010030",
                    "target_usage": "0x00010030",
                    "layers": [
                        2
                    ],
                    "sticky": false,
                    "scaling": 250
                },
                {
                    "source_usage": "0x00010031",
                    "target_usage": "0x00010031",
                    "layers": [
                        2
                    ],
                    "sticky": false,
                    "scaling": 250
                }
            ],
            "macros": [
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                []
            ]
        }
    },
    {
        'description': 'scroll wheel smoothly adjusts DPI',
        'config': {
            "version": 13,
            "unmapped_passthrough_layers": [
                0,
                1,
                2,
                3,
                4,
                5,
                6,
                7
            ],
            "partial_scroll_timeout": 1000000,
            "tap_hold_threshold": 200000,
            "gpio_debounce_time_ms": 5,
            "interval_override": 0,
            "our_descriptor_number": 0,
            "ignore_auth_dev_inputs": false,
            "macro_entry_duration": 1,
            "gpio_output_mode": 0,
            "mappings": [
                {
                    "target_usage": "0x00010030",
                    "source_usage": "0xfff30002",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00010031",
                    "source_usage": "0xfff30003",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00000000",
                    "source_usage": "0x00010038",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                }
            ],
            "macros": [
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                []
            ],
            "expressions": [
                "1000 recall /* register 1 is current scaling factor */ eol dup 0 eq add /* on startup it's 0, so make it 1 */ eol eol 0x00010038 input_state /* scroll wheel */ eol 1000 eq /* if scroll up */ eol 1100 mul /* increase scaling factor */ eol 0x00010038 input_state /* scroll wheel */ eol -1000 eq /* if scroll down */ eol 900 mul /* decrease scaling factor */ eol add eol dup 0 eq add /* if result is 0, make it 1, meaning no change */ eol eol mul /* multiply scaling factor by either 1.1, 0.9, or 1.0 */ eol 100 10000 clamp /* restrict scaling factor to 0.1-10 range */ eol 1000 store /* put result back in register 1 */ eol eol /*\nFor real-life use we'd want the scaling factor to max out at 1\nand set the hardware mouse DPI to maximum instead.\nBut for demonstration purposes this will do.\n*/",
                "0x00010030 input_state /* raw mouse X */ eol 1000 recall /* current scaling factor */ eol mul eol /* result used as mouse X */",
                "0x00010031 input_state /* raw mouse Y */ eol 1000 recall /* current scaling factor */ eol mul eol /* result used as mouse Y */",
                "",
                "",
                "",
                "",
                ""
            ],
            "quirks": []
        }
    },
    {
        'description': 'macros: double-click and "Hello, world!"',
        'config': {
            "version": 5,
            "unmapped_passthrough_layers": [
                0,
                1,
                2,
                3
            ],
            "partial_scroll_timeout": 1000000,
            "tap_hold_threshold": 200000,
            "interval_override": 0,
            "mappings": [
                {
                    "source_usage": "0x00090002",
                    "target_usage": "0xfff20001",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000
                },
                {
                    "source_usage": "0x00090003",
                    "target_usage": "0xfff20002",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000
                }
            ],
            "macros": [
                [
                    [
                        "0x00090001"
                    ],
                    [],
                    [
                        "0x00090001"
                    ]
                ],
                [
                    [
                        "0x000700e1",
                        "0x0007000b"
                    ],
                    [
                        "0x00070008"
                    ],
                    [
                        "0x0007000f"
                    ],
                    [],
                    [
                        "0x0007000f"
                    ],
                    [
                        "0x00070012"
                    ],
                    [
                        "0x00070036"
                    ],
                    [
                        "0x0007002c"
                    ],
                    [
                        "0x0007001a"
                    ],
                    [
                        "0x00070012"
                    ],
                    [
                        "0x00070015"
                    ],
                    [
                        "0x0007000f"
                    ],
                    [
                        "0x00070007"
                    ],
                    [
                        "0x000700e1",
                        "0x0007001e"
                    ]
                ],
                [],
                [],
                [],
                [],
                [],
                []
            ]
        }
    },
    {
        'description': 'tap-hold: middle button is middle-click when clicked, but switches layer when held',
        'config': {
            "version": 5,
            "unmapped_passthrough_layers": [
                0,
                1,
                2,
                3
            ],
            "partial_scroll_timeout": 1000000,
            "tap_hold_threshold": 200000,
            "interval_override": 0,
            "mappings": [
                {
                    "source_usage": "0x00090003",
                    "target_usage": "0x00090003",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": true,
                    "hold": false,
                    "scaling": 1000
                },
                {
                    "source_usage": "0x00090003",
                    "target_usage": "0xfff10001",
                    "layers": [
                        0,
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": true,
                    "scaling": 1000
                },
                {
                    "source_usage": "0x00010030",
                    "target_usage": "0x000c0238",
                    "layers": [
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 10
                },
                {
                    "source_usage": "0x00010031",
                    "target_usage": "0x00010038",
                    "layers": [
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 10
                }
            ],
            "macros": [
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                []
            ]
        }
    },
    {
        'description': 'tap-hold: middle button activates layer permanently when tapped, temporarily when held',
        'config': {
            "version": 5,
            "unmapped_passthrough_layers": [
                0,
                1,
                2,
                3
            ],
            "partial_scroll_timeout": 1000000,
            "tap_hold_threshold": 200000,
            "interval_override": 0,
            "mappings": [
                {
                    "source_usage": "0x00090003",
                    "target_usage": "0xfff10001",
                    "layers": [
                        0
                    ],
                    "sticky": true,
                    "tap": true,
                    "hold": false,
                    "scaling": 1000
                },
                {
                    "source_usage": "0x00090003",
                    "target_usage": "0xfff10001",
                    "layers": [
                        0,
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": true,
                    "scaling": 1000
                },
                {
                    "source_usage": "0x00010030",
                    "target_usage": "0x000c0238",
                    "layers": [
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 10
                },
                {
                    "source_usage": "0x00010031",
                    "target_usage": "0x00010038",
                    "layers": [
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 10
                }
            ],
            "macros": [
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                []
            ]
        }
    },
    {
        'description': 'expressions: gamepad-to-mouse adapter',
        'config': {
            "version": 6,
            "unmapped_passthrough_layers": [],
            "partial_scroll_timeout": 1000000,
            "interval_override": 0,
            "tap_hold_threshold": 200000,
            "mappings": [
                {
                    "target_usage": "0x00090001",
                    "source_usage": "0x00090002",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false
                },
                {
                    "target_usage": "0x00090002",
                    "source_usage": "0x00090003",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false
                },
                {
                    "target_usage": "0x00010030",
                    "source_usage": "0xfff30001",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false
                },
                {
                    "target_usage": "0x00010031",
                    "source_usage": "0xfff30002",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false
                },
                {
                    "target_usage": "0x00010038",
                    "source_usage": "0xfff30003",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false
                },
                {
                    "target_usage": "0x00010038",
                    "source_usage": "0xfff30004",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false
                },
                {
                    "target_usage": "0x00010030",
                    "source_usage": "0xfff30005",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false
                },
                {
                    "target_usage": "0x00010031",
                    "source_usage": "0xfff30006",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false
                }
            ],
            "macros": [
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                []
            ],
            "expressions": [
                "0x00010030 input_state -128000 add dup abs 10000 gt mul 25 mul",
                "0x00010031 input_state -128000 add dup abs 10000 gt mul 25 mul",
                "0x00010033 input_state 1 mul 0x00010034 input_state -1 mul add 250 mul",
                "0x00010035 input_state -128000 add dup abs 10000 gt mul -1 mul 250 mul",
                "0x00010039 input_state 7000 gt not 0x00010039 input_state 45000 mul sin 1000 mul mul",
                "0x00010039 input_state 7000 gt not 0x00010039 input_state 45000 mul cos -1000 mul mul",
                "",
                ""
            ]
        }
    },
    {
        'description': 'expressions: middle button enables mouse jiggler',
        'config': {
            "version": 6,
            "unmapped_passthrough_layers": [
                0,
                1,
                2,
                3
            ],
            "partial_scroll_timeout": 1000000,
            "interval_override": 0,
            "tap_hold_threshold": 200000,
            "mappings": [
                {
                    "target_usage": "0xfff10001",
                    "source_usage": "0x00090003",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": true,
                    "tap": false,
                    "hold": false
                },
                {
                    "target_usage": "0x00010030",
                    "source_usage": "0xfff30001",
                    "scaling": 1000,
                    "layers": [
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false
                },
                {
                    "target_usage": "0x00010031",
                    "source_usage": "0xfff30002",
                    "scaling": 1000,
                    "layers": [
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false
                }
            ],
            "macros": [
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                []
            ],
            "expressions": [
                "time 2000000 mod 500000 gt not time 500000 mod 720 mul cos mul",
                "time 2000000 mod 500000 gt not time 500000 mod 720 mul sin -1000 mul mul",
                "",
                "",
                "",
                "",
                "",
                ""
            ]
        }
    },
    {
        'description': 'swap caps lock and scroll lock LEDs',
        'config': {
            "version": 7,
            "unmapped_passthrough_layers": [
                0,
                1,
                2,
                3
            ],
            "partial_scroll_timeout": 1000000,
            "tap_hold_threshold": 200000,
            "gpio_debounce_time_ms": 5,
            "interval_override": 0,
            "mappings": [
                {
                    "source_usage": "0x00080002",
                    "target_usage": "0x00080003",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000
                },
                {
                    "source_usage": "0x00080003",
                    "target_usage": "0x00080002",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000
                }
            ],
            "macros": [
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                []
            ],
            "expressions": [
                "",
                "",
                "",
                "",
                "",
                "",
                "",
                ""
            ]
        }
    },
    {
        'description': 'caps lock LED blinks when on',
        'config': {
            "version": 7,
            "unmapped_passthrough_layers": [
                0,
                1,
                2,
                3
            ],
            "partial_scroll_timeout": 1000000,
            "tap_hold_threshold": 200000,
            "gpio_debounce_time_ms": 5,
            "interval_override": 0,
            "mappings": [
                {
                    "source_usage": "0xfff30001",
                    "target_usage": "0x00080002",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000
                }
            ],
            "macros": [
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                []
            ],
            "expressions": [
                "0x00080002 input_state_binary time 600000 mod 200000 gt mul",
                "",
                "",
                "",
                "",
                "",
                "",
                ""
            ]
        }
    },
    {
        'description': 'caps lock LED on when layer 1 active',
        'config': {
            "version": 7,
            "unmapped_passthrough_layers": [
                0,
                1,
                2,
                3
            ],
            "partial_scroll_timeout": 1000000,
            "tap_hold_threshold": 200000,
            "gpio_debounce_time_ms": 5,
            "interval_override": 0,
            "mappings": [
                {
                    "source_usage": "0xfff30001",
                    "target_usage": "0x00080002",
                    "layers": [
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000
                },
                {
                    "source_usage": "0x000700e1",
                    "target_usage": "0xfff10001",
                    "layers": [
                        0,
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000
                }
            ],
            "macros": [
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                []
            ],
            "expressions": [
                "1000",
                "",
                "",
                "",
                "",
                "",
                "",
                ""
            ]
        }
    },
    {
        'description': 'expressions: auto-click left mouse button when cursor stops moving',
        'config': {
            "version": 8,
            "unmapped_passthrough_layers": [
                0,
                1,
                2,
                3
            ],
            "partial_scroll_timeout": 1000000,
            "tap_hold_threshold": 200000,
            "gpio_debounce_time_ms": 5,
            "interval_override": 0,
            "mappings": [
                {
                    "source_usage": "0xfff30003",
                    "target_usage": "0x00090001",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000
                }
            ],
            "macros": [
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                []
            ],
            "expressions": [
                "0x00010030 input_state dup mul 0x00010031 input_state dup mul add sqrt dup 1000 store 3000 recall add 3000 store",
                "2000 recall 1000 add 1000 recall 0 eq mul 2000 store",
                "2000 recall 500000 gt 2000 recall 600000 gt not mul 3000 recall 200000 gt mul 2000 recall 600000 gt not 3000 recall mul 3000 store",
                "",
                "",
                "",
                "",
                ""
            ]
        }
    },
    {
        'description': 'advanced autoclicker: use screen corners for right click, double click, click-drag',
        'config': {
            "version": 9,
            "unmapped_passthrough_layers": [
                0,
                1,
                2,
                3
            ],
            "partial_scroll_timeout": 1000000,
            "tap_hold_threshold": 200000,
            "gpio_debounce_time_ms": 5,
            "interval_override": 0,
            "our_descriptor_number": 1,
            "ignore_auth_dev_inputs": false,
            "mappings": [
                {
                    "target_usage": "0x00010030",
                    "source_usage": "0xfff30001",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false
                },
                {
                    "target_usage": "0x00010031",
                    "source_usage": "0xfff30002",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false
                },
                {
                    "target_usage": "0x00000000",
                    "source_usage": "0x00010030",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false
                },
                {
                    "target_usage": "0x00000000",
                    "source_usage": "0x00010031",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false
                },
                {
                    "target_usage": "0x00090001",
                    "source_usage": "0xfff30006",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false
                },
                {
                    "target_usage": "0x00090002",
                    "source_usage": "0xfff30007",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false
                }
            ],
            "macros": [
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                []
            ],
            "expressions": [
                "0x00010030 input_state 9000 mul 1000 recall add 0 32767000 clamp dup 1000 store",
                "0x00010031 input_state 16000 mul 2000 recall add 0 32767000 clamp dup 2000 store",
                "0x00010030 input_state dup mul 0x00010031 input_state dup mul add sqrt dup 4000 recall add 4000 store 0 eq 5000 recall 1000 add mul 5000 store",
                "1000 recall 0 eq 2000 recall 0 eq mul 1000 mul 1000 recall 32767000 eq 2000 recall 0 eq mul 2000 mul add 1000 recall 0 eq 2000 recall 32767000 eq mul 3000 mul add 1000 recall 32767000 eq 2000 recall 32767000 eq mul 4000 mul add dup 0 eq 6000 recall mul add 6000 store",
                "",
                "5000 recall 500000 gt 5000 recall 600000 gt not mul 4000 recall 200000 gt mul 6000 recall 1000 eq mul 6000 recall 5000 eq add 5000 recall 500000 gt 5000 recall 533000 gt not mul 5000 recall 566000 gt 5000 recall 600000 gt not mul add 4000 recall 200000 gt mul 6000 recall 3000 eq mul add",
                "5000 recall 500000 gt 5000 recall 600000 gt not mul 4000 recall 200000 gt mul 6000 recall 2000 eq mul",
                "5000 recall 600000 gt not 4000 recall mul 4000 store 5000 recall 600000 eq not 6000 recall mul 5000 recall 600000 eq 6000 recall 4000 eq not 1000 mul 6000 recall 4000 eq 5000 mul add mul add 6000 store"
            ]
        }
    },
    {
        'description': 'absolute mouse: basic functionality',
        'config': {
            "version": 9,
            "unmapped_passthrough_layers": [
                0,
                1,
                2,
                3
            ],
            "partial_scroll_timeout": 1000000,
            "tap_hold_threshold": 200000,
            "gpio_debounce_time_ms": 5,
            "interval_override": 0,
            "our_descriptor_number": 1,
            "ignore_auth_dev_inputs": false,
            "mappings": [
                {
                    "target_usage": "0x00010030",
                    "source_usage": "0xfff30001",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false
                },
                {
                    "target_usage": "0x00010031",
                    "source_usage": "0xfff30002",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false
                }
            ],
            "macros": [
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                []
            ],
            "expressions": [
                "0x00010030 input_state 9000 mul 1000 recall add 0 32767000 clamp dup 1000 store",
                "0x00010031 input_state 16000 mul 2000 recall add 0 32767000 clamp dup 2000 store",
                "",
                "",
                "",
                "",
                "",
                ""
            ]
        }
    },
    {
        'description': 'keyboard adapter for Switch with 15 degree nudges and tilt',
        'config': {
            "version": 16,
            "unmapped_passthrough_layers": [],
            "partial_scroll_timeout": 1000000,
            "tap_hold_threshold": 200000,
            "gpio_debounce_time_ms": 5,
            "interval_override": 0,
            "our_descriptor_number": 2,
            "ignore_auth_dev_inputs": false,
            "input_labels": 0,
            "mappings": [
                {
                    "target_usage": "0x00010032",
                    "source_usage": "0xfff30007",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00010035",
                    "source_usage": "0xfff30008",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00010030",
                    "source_usage": "0xfff30004",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00010031",
                    "source_usage": "0xfff30005",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090003",
                    "source_usage": "0x0007000d",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090002",
                    "source_usage": "0x0007000e",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090006",
                    "source_usage": "0x0007000f",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090008",
                    "source_usage": "0x00070033",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090005",
                    "source_usage": "0x00070010",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090007",
                    "source_usage": "0x00070036",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090004",
                    "source_usage": "0x00070037",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x0009000a",
                    "source_usage": "0x00070028",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x0009000a",
                    "source_usage": "0x0007002e",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090009",
                    "source_usage": "0x0007002d",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x0009000d",
                    "source_usage": "0x0007004a",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x0009000b",
                    "source_usage": "0x00070009",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x0009000c",
                    "source_usage": "0x0007000b",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090001",
                    "source_usage": "0x0007002c",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090004",
                    "source_usage": "0x0007004f",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x0009000e",
                    "source_usage": "0x0007004d",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090003",
                    "source_usage": "0x0007005c",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090002",
                    "source_usage": "0x0007005d",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090006",
                    "source_usage": "0x0007005e",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090008",
                    "source_usage": "0x00070057",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090005",
                    "source_usage": "0x0007005f",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090007",
                    "source_usage": "0x00070060",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x0009000c",
                    "source_usage": "0x00070061",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                }
            ],
            "macros": [
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                []
            ],
            "expressions": [
                "",
                "",
                "0x00070004 input_state_binary 1 mul 0x00070007 input_state_binary 2 mul bitwise_or 0x0007001a input_state_binary 4 mul bitwise_or 0x00070016 input_state_binary 8 mul bitwise_or 1000 store -1000 1000 recall 1 eq 7000 mul add 1000 recall 2 eq 3000 mul add 1000 recall 4 eq 1000 mul add 1000 recall 8 eq 5000 mul add 1000 recall 5 eq 8000 mul add 1000 recall 6 eq 2000 mul add 1000 recall 10 eq 4000 mul add 1000 recall 9 eq 6000 mul add 1000 recall 7 eq 1000 mul add 1000 recall 11 eq 5000 mul add 1000 recall 13 eq 7000 mul add 1000 recall 14 eq 3000 mul add 45000 mul dup dup dup dup 135000 gt 2000 mul -1000 add 4000 store 315000 add 360000 mod 135000 gt 2000 mul -1000 add 5000 store -45000 eq not dup 7000 store 0 0x00070014 input_state_binary 15000 mul 4000 recall mul add 0x00070008 input_state_binary 15000 mul 5000 recall mul add 0x0007001d input_state_binary -15000 mul 5000 recall mul add 0x00070006 input_state_binary -15000 mul 4000 recall mul add mul add 3000 store 0x000700e1 input_state_binary -500 mul 1000 add 6000 store",
                "3000 recall sin 127000 mul 6000 recall mul 7000 recall mul 128000 add",
                "3000 recall cos -127000 mul 6000 recall mul 7000 recall mul 128000 add",
                "0x00070018 input_state_binary 0x00070059 input_state_binary bitwise_or 1 mul 0x00070012 input_state_binary 0x0007005b input_state_binary bitwise_or 2 mul bitwise_or 0x00070013 input_state_binary 0x00070058 input_state_binary bitwise_or 4 mul bitwise_or 0x0007000c input_state_binary 0x0007005a input_state_binary bitwise_or 8 mul bitwise_or 10000 store -1000 10000 recall 1 eq 7000 mul add 10000 recall 2 eq 3000 mul add 10000 recall 4 eq 1000 mul add 10000 recall 8 eq 5000 mul add 10000 recall 5 eq 8000 mul add 10000 recall 6 eq 2000 mul add 10000 recall 10 eq 4000 mul add 10000 recall 9 eq 6000 mul add 10000 recall 7 eq 1000 mul add 10000 recall 11 eq 5000 mul add 10000 recall 13 eq 7000 mul add 10000 recall 14 eq 3000 mul add 45000 mul dup -45000 eq not 8000 store 9000 store",
                "9000 recall sin 127000 mul 8000 recall mul 128000 add",
                "9000 recall cos -127000 mul 8000 recall mul 128000 add"
            ],
            "macro_entry_duration": 1,
            "gpio_output_mode": 0,
            "quirks": []
        }
    },
    {
        'description': 'Stadia controller adapter for PS4',
        'config': {
            "version": 16,
            "unmapped_passthrough_layers": [],
            "partial_scroll_timeout": 1000000,
            "tap_hold_threshold": 200000,
            "gpio_debounce_time_ms": 5,
            "interval_override": 0,
            "our_descriptor_number": 3,
            "ignore_auth_dev_inputs": true,
            "input_labels": 0,
            "mappings": [
                {
                    "target_usage": "0x00010030",
                    "source_usage": "0x00010030",
                    "source_name": "Left stick X",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00010031",
                    "source_usage": "0x00010031",
                    "source_name": "Left stick Y",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00010032",
                    "source_usage": "0x00010032",
                    "source_name": "Right stick X",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00010035",
                    "source_usage": "0x00010035",
                    "source_name": "Right stick Y",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0xfff90001",
                    "source_usage": "0xfff90001",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0xfff90002",
                    "source_usage": "0xfff90002",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0xfff90003",
                    "source_usage": "0xfff90003",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0xfff90004",
                    "source_usage": "0xfff90004",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00010033",
                    "source_usage": "0x000200c5",
                    "source_name": "L2 axis",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00010034",
                    "source_usage": "0x000200c4",
                    "source_name": "R2 axis",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090002",
                    "source_usage": "0x00090001",
                    "source_name": "A",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090003",
                    "source_usage": "0x00090002",
                    "source_name": "B",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090004",
                    "source_usage": "0x00090005",
                    "source_name": "Y",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090001",
                    "source_usage": "0x00090004",
                    "source_name": "X",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090005",
                    "source_usage": "0x00090007",
                    "source_name": "L1",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090006",
                    "source_usage": "0x00090008",
                    "source_name": "R1",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090007",
                    "source_usage": "0x00090014",
                    "source_name": "L2",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090008",
                    "source_usage": "0x00090013",
                    "source_name": "R2",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090009",
                    "source_usage": "0x0009000b",
                    "source_name": "Options",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x0009000a",
                    "source_usage": "0x0009000c",
                    "source_name": "Menu",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x0009000b",
                    "source_usage": "0x0009000e",
                    "source_name": "L3",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x0009000c",
                    "source_usage": "0x0009000f",
                    "source_name": "R3",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x0009000d",
                    "source_usage": "0x0009000d",
                    "source_name": "Stadia",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x0009000e",
                    "source_usage": "0x00090011",
                    "source_name": "Assistant",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x0009000e",
                    "source_usage": "0x00090012",
                    "source_name": "Capture",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                }
            ],
            "macros": [
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                []
            ],
            "expressions": [
                "",
                "",
                "",
                "",
                "",
                "",
                "",
                ""
            ],
            "macro_entry_duration": 1,
            "gpio_output_mode": 0,
            "quirks": []
        }
    },
    {
        'description': 'keyboard adapter for PS4: arrows=D-pad, WASD=left stick, numpad=right stick',
        'config': {
            "version": 16,
            "unmapped_passthrough_layers": [
                0,
                1,
                2,
                3
            ],
            "partial_scroll_timeout": 1000000,
            "tap_hold_threshold": 200000,
            "gpio_debounce_time_ms": 5,
            "interval_override": 0,
            "our_descriptor_number": 3,
            "ignore_auth_dev_inputs": true,
            "macro_entry_duration": 1,
            "gpio_output_mode": 0,
            "input_labels": 0,
            "mappings": [
                {
                    "target_usage": "0xfff90001",
                    "source_usage": "0x00070050",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0xfff90002",
                    "source_usage": "0x0007004f",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0xfff90003",
                    "source_usage": "0x00070052",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0xfff90004",
                    "source_usage": "0x00070051",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00010030",
                    "source_usage": "0xfff30003",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00010031",
                    "source_usage": "0xfff30004",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00010032",
                    "source_usage": "0xfff30006",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00010035",
                    "source_usage": "0xfff30007",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090002",
                    "source_usage": "0x0007000d",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090003",
                    "source_usage": "0x0007000e",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090001",
                    "source_usage": "0x0007000c",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090004",
                    "source_usage": "0x00070012",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090006",
                    "source_usage": "0x00070013",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090008",
                    "source_usage": "0x0007000f",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00010034",
                    "source_usage": "0x0007000f",
                    "scaling": 255000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090005",
                    "source_usage": "0x0007002f",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090007",
                    "source_usage": "0x00070033",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00010033",
                    "source_usage": "0x00070033",
                    "scaling": 255000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x0009000b",
                    "source_usage": "0x00070009",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x0009000c",
                    "source_usage": "0x0007000b",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x0009000d",
                    "source_usage": "0x0007004a",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090009",
                    "source_usage": "0x00070046",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x0009000a",
                    "source_usage": "0x00070028",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x0009000e",
                    "source_usage": "0x0007002a",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                }
            ],
            "macros": [
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                []
            ],
            "expressions": [
                "",
                "0x00070004 input_state_binary /* A */ eol 0x00070007 input_state_binary /* D */ eol 0x0007001a input_state_binary /* W */ eol 0x00070016 input_state_binary /* S */ eol dpad /* 0=up, 1=up+right, 2=right, ..., 8=neutral */ eol 45000 mul /* angle in degrees */ eol dup eol 360000 eq not 3000 store /* is any of the keys pressed? */ eol 4000 store /* 0..360 angle */",
                "4000 recall /* 0..360 angle */ eol sin eol 127000 mul eol 3000 recall /* is any of the keys pressed? */ eol mul eol 128000 add eol /* used as left stick X */",
                "4000 recall /* 0..360 angle */ eol cos eol -127000 eol mul eol 3000 recall /* is any of the keys pressed? */ eol mul eol 128000 add eol /* used as left stick Y */",
                "0x0007005c input_state_binary /* numpad 4 */ eol 0x0007005e input_state_binary /* numpad 6 */ eol 0x00070060 input_state_binary /* numpad 8 */ eol 0x0007005d input_state_binary /* numpad 5 */ eol dpad /* 0=up, 1=up+right, 2=right, ..., 8=neutral */ eol 45000 mul /* angle in degrees */ eol dup eol 360000 eq not 6000 store /* is any of the keys pressed? */ eol 7000 store /* 0..360 angle */",
                "7000 recall /* 0..360 angle */ eol sin eol 127000 mul eol 6000 recall /* is any of the keys pressed? */ eol mul eol 128000 add eol /* used as right stick X */",
                "7000 recall /* 0..360 angle */ eol cos eol -127000 mul eol 6000 recall /* is any of the keys pressed? */ eol mul eol 128000 add eol /* used as right stick Y */",
                ""
            ],
            "quirks": []
        }
    },
    {
        'description': 'mouse to analog stick',
        'config': {
            "version": 16,
            "unmapped_passthrough_layers": [],
            "partial_scroll_timeout": 1000000,
            "tap_hold_threshold": 200000,
            "gpio_debounce_time_ms": 5,
            "interval_override": 0,
            "our_descriptor_number": 2,
            "ignore_auth_dev_inputs": false,
            "input_labels": 0,
            "mappings": [
                {
                    "target_usage": "0x00010030",
                    "source_usage": "0xfff30001",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00010031",
                    "source_usage": "0xfff30002",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                }
            ],
            "macros": [
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                []
            ],
            "expressions": [
                "0x00010030 input_state eol 1000 recall eol add eol -127000 127000 clamp eol 980 mul eol dup eol 1000 store eol 128000 add",
                "0x00010031 input_state eol 2000 recall eol add eol -127000 127000 clamp eol 980 mul eol dup eol 2000 store eol 128000 add",
                "",
                "",
                "",
                "",
                "",
                ""
            ],
            "macro_entry_duration": 1,
            "gpio_output_mode": 0,
            "quirks": []
        }
    },
    {
        'description': 'mouse movement to arrow keys, but number of keypresses is proportional to distance',
        'config': {
            "version": 9,
            "unmapped_passthrough_layers": [],
            "partial_scroll_timeout": 1000000,
            "tap_hold_threshold": 200000,
            "gpio_debounce_time_ms": 5,
            "interval_override": 0,
            "our_descriptor_number": 0,
            "ignore_auth_dev_inputs": false,
            "mappings": [
                {
                    "target_usage": "0x0007004f",
                    "source_usage": "0xfff30002",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false
                },
                {
                    "target_usage": "0x00070050",
                    "source_usage": "0xfff30003",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false
                }
            ],
            "macros": [
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                []
            ],
            "expressions": [
                "0x00010030 input_state 100 mul 1000 recall add 1000 store",
                "1000 recall 1000 gt 2000 store 1000 recall -1000 add 0 1000000 clamp 2000 recall mul 1000 recall 2000 recall not mul add 1000 store time 2000 mod 2000 recall mul",
                "1000 recall -1000 gt not 3000 store 1000 recall 1000 add -1000000 0 clamp 3000 recall mul 1000 recall 3000 recall not mul add 1000 store time 2000 mod 3000 recall mul",
                "",
                "",
                "",
                "",
                ""
            ]
        }
    },
    {
        'description': 'Xbox controller (Bluetooth) adapter for Switch',
        'config': {
            "version": 17,
            "unmapped_passthrough_layers": [],
            "partial_scroll_timeout": 1000000,
            "tap_hold_threshold": 200000,
            "gpio_debounce_time_ms": 5,
            "interval_override": 0,
            "our_descriptor_number": 2,
            "ignore_auth_dev_inputs": false,
            "input_labels": 1,
            "mappings": [
                {
                    "target_usage": "0xfff90001",
                    "source_usage": "0xfff50001",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0xfff90002",
                    "source_usage": "0xfff50002",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0xfff90003",
                    "source_usage": "0xfff50003",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0xfff90004",
                    "source_usage": "0xfff50004",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00010030",
                    "source_usage": "0x00010030",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00010031",
                    "source_usage": "0x00010031",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00010032",
                    "source_usage": "0x00010032",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00010035",
                    "source_usage": "0x00010035",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090002",
                    "source_usage": "0x00090001",
                    "source_name": "A",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090003",
                    "source_usage": "0x00090002",
                    "source_name": "B",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090001",
                    "source_usage": "0x00090004",
                    "source_name": "X",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090004",
                    "source_usage": "0x00090005",
                    "source_name": "Y",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090005",
                    "source_usage": "0x00090007",
                    "source_name": "LB",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090006",
                    "source_usage": "0x00090008",
                    "source_name": "RB",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090007",
                    "source_usage": "0xfff30002",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090008",
                    "source_usage": "0xfff30003",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x0009000b",
                    "source_usage": "0x0009000e",
                    "source_name": "LSB",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x0009000c",
                    "source_usage": "0x0009000f",
                    "source_name": "RSB",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090009",
                    "source_usage": "0x0009000b",
                    "source_name": "View",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x0009000a",
                    "source_usage": "0x0009000c",
                    "source_name": "Menu",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x0009000d",
                    "source_usage": "0x0009000d",
                    "source_name": "Guide",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                }
            ],
            "macros": [
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                []
            ],
            "expressions": [
                "/*\nXbox controllers in Bluetooth mode report dpad directions as 1..8\n(N, NE, E, SE, S, SW, W, NW) with 0 as neutral.\n*/ eol eol 0x00010039 input_state 6000 eq eol 0x00010039 input_state 7000 eq bitwise_or eol 0x00010039 input_state 8000 eq bitwise_or eol 1000 store /* left */ eol eol 0x00010039 input_state 2000 eq eol 0x00010039 input_state 3000 eq bitwise_or eol 0x00010039 input_state 4000 eq bitwise_or eol 2000 store /* right */ eol eol 0x00010039 input_state 8000 eq eol 0x00010039 input_state 1000 eq bitwise_or eol 0x00010039 input_state 2000 eq bitwise_or eol 3000 store /* up */ eol eol 0x00010039 input_state 4000 eq eol 0x00010039 input_state 5000 eq bitwise_or eol 0x00010039 input_state 6000 eq bitwise_or eol 4000 store /* down */",
                "0x000200c5 input_state_scaled eol 63000 gt",
                "0x000200c4 input_state_scaled eol 63000 gt",
                "",
                "",
                "",
                "",
                ""
            ],
            "macro_entry_duration": 1,
            "gpio_output_mode": 0,
            "quirks": []
        }
    },
    {
        'description': 'Xbox controller (USB) adapter for Switch',
        'config': {
            "version": 17,
            "unmapped_passthrough_layers": [],
            "partial_scroll_timeout": 1000000,
            "tap_hold_threshold": 200000,
            "gpio_debounce_time_ms": 5,
            "interval_override": 0,
            "our_descriptor_number": 2,
            "ignore_auth_dev_inputs": false,
            "input_labels": 1,
            "mappings": [
                {
                    "target_usage": "0xfff90001",
                    "source_usage": "0x0009000b",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0xfff90002",
                    "source_usage": "0x0009000c",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0xfff90003",
                    "source_usage": "0x00090009",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0xfff90004",
                    "source_usage": "0x0009000a",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00010030",
                    "source_usage": "0x00010030",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00010031",
                    "source_usage": "0x00010031",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00010032",
                    "source_usage": "0x00010032",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00010035",
                    "source_usage": "0x00010035",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090002",
                    "source_usage": "0x00090005",
                    "source_name": "A",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090003",
                    "source_usage": "0x00090006",
                    "source_name": "B",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090001",
                    "source_usage": "0x00090007",
                    "source_name": "X",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090004",
                    "source_usage": "0x00090008",
                    "source_name": "Y",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090005",
                    "source_usage": "0x0009000d",
                    "source_name": "LB",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090006",
                    "source_usage": "0x0009000e",
                    "source_name": "RB",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090007",
                    "source_usage": "0xfff30001",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090008",
                    "source_usage": "0xfff30002",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x0009000b",
                    "source_usage": "0x0009000f",
                    "source_name": "LSB",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x0009000c",
                    "source_usage": "0x00090010",
                    "source_name": "RSB",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090009",
                    "source_usage": "0x00090004",
                    "source_name": "View",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x0009000a",
                    "source_usage": "0x00090003",
                    "source_name": "Menu",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x0009000d",
                    "source_usage": "0x00090011",
                    "source_name": "Guide",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                }
            ],
            "macros": [
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                []
            ],
            "expressions": [
                "0x000200c5 input_state_scaled eol 63000 gt",
                "0x000200c4 input_state_scaled eol 63000 gt",
                "",
                "",
                "",
                "",
                "",
                ""
            ],
            "macro_entry_duration": 1,
            "gpio_output_mode": 0,
            "quirks": []
        }
    },
    {
        'description': 'mirrorboard: a one-handed keyboard layout',
        'config': {
            "version": 12,
            "unmapped_passthrough_layers": [
                0,
                1,
                2,
                3,
                4,
                5,
                6,
                7
            ],
            "partial_scroll_timeout": 1000000,
            "tap_hold_threshold": 200000,
            "gpio_debounce_time_ms": 5,
            "interval_override": 0,
            "our_descriptor_number": 0,
            "ignore_auth_dev_inputs": false,
            "macro_entry_duration": 1,
            "gpio_output_mode": 0,
            "mappings": [
                {
                    "target_usage": "0xfff10001",
                    "source_usage": "0x00070039",
                    "scaling": 1000,
                    "layers": [
                        0,
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x0007002a",
                    "source_usage": "0x00070035",
                    "scaling": 1000,
                    "layers": [
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00070028",
                    "source_usage": "0x0007002c",
                    "scaling": 1000,
                    "layers": [
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00070013",
                    "source_usage": "0x00070014",
                    "scaling": 1000,
                    "layers": [
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00070012",
                    "source_usage": "0x0007001a",
                    "scaling": 1000,
                    "layers": [
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x0007000c",
                    "source_usage": "0x00070008",
                    "scaling": 1000,
                    "layers": [
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00070018",
                    "source_usage": "0x00070015",
                    "scaling": 1000,
                    "layers": [
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x0007001c",
                    "source_usage": "0x00070017",
                    "scaling": 1000,
                    "layers": [
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00070017",
                    "source_usage": "0x0007001c",
                    "scaling": 1000,
                    "layers": [
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00070034",
                    "source_usage": "0x00070004",
                    "scaling": 1000,
                    "layers": [
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x0007000f",
                    "source_usage": "0x00070016",
                    "scaling": 1000,
                    "layers": [
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x0007000e",
                    "source_usage": "0x00070007",
                    "scaling": 1000,
                    "layers": [
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x0007000d",
                    "source_usage": "0x00070009",
                    "scaling": 1000,
                    "layers": [
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x0007000b",
                    "source_usage": "0x0007000a",
                    "scaling": 1000,
                    "layers": [
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x0007000a",
                    "source_usage": "0x0007000b",
                    "scaling": 1000,
                    "layers": [
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00070037",
                    "source_usage": "0x0007001d",
                    "scaling": 1000,
                    "layers": [
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00070036",
                    "source_usage": "0x0007001b",
                    "scaling": 1000,
                    "layers": [
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00070010",
                    "source_usage": "0x00070006",
                    "scaling": 1000,
                    "layers": [
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00070011",
                    "source_usage": "0x00070019",
                    "scaling": 1000,
                    "layers": [
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                }
            ],
            "macros": [
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                []
            ],
            "expressions": [
                "",
                "",
                "",
                "",
                "",
                "",
                "",
                ""
            ],
            "quirks": []
        }
    },
    {
        'description': 'custom board v8: analog stick',
        'config': {
            "version": 15,
            "unmapped_passthrough_layers": [
                0,
                1,
                2,
                3,
                4,
                5,
                6,
                7
            ],
            "partial_scroll_timeout": 1000000,
            "tap_hold_threshold": 200000,
            "gpio_debounce_time_ms": 5,
            "interval_override": 0,
            "our_descriptor_number": 5,
            "ignore_auth_dev_inputs": false,
            "macro_entry_duration": 1,
            "gpio_output_mode": 0,
            "mappings": [
                {
                    "target_usage": "0x00010030",
                    "source_usage": "0xfff30001",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00010031",
                    "source_usage": "0xfff30002",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                }
            ],
            "macros": [
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                []
            ],
            "expressions": [
                "0xfff80000 input_state /* 12-bit ADC value */ eol 2048000 /* X neutral point */ eol sub eol dup sign swap abs eol 50000 /* per-axis deadzone */ eol sub relu eol mul /* restore sign */ eol 100 mul /* scale */ eol 128000 add /* 128 is neutral on output */ eol 0 255000 clamp /* output range is 0-255 */",
                "0xfff80001 input_state /* 12-bit ADC value */ eol 2048000 /* Y neutral point */ eol sub eol dup sign swap abs eol 50000 /* per-axis deadzone */ eol sub relu eol mul /* restore sign */ eol -100 mul /* scale and invert */ eol 128000 add /* 128 is neutral on output */ eol 0 255000 clamp /* output range is 0-255 */",
                "",
                "",
                "",
                "",
                "",
                ""
            ],
            "quirks": []
        }
    },
    {
        'description': 'custom board v8: analog stick as mouse',
        'config': {
            "version": 15,
            "unmapped_passthrough_layers": [
                0,
                1,
                2,
                3,
                4,
                5,
                6,
                7
            ],
            "partial_scroll_timeout": 1000000,
            "tap_hold_threshold": 200000,
            "gpio_debounce_time_ms": 5,
            "interval_override": 0,
            "our_descriptor_number": 0,
            "ignore_auth_dev_inputs": false,
            "macro_entry_duration": 1,
            "gpio_output_mode": 0,
            "mappings": [
                {
                    "target_usage": "0x00010030",
                    "source_usage": "0xfff30001",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00010031",
                    "source_usage": "0xfff30002",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                }
            ],
            "macros": [
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                []
            ],
            "expressions": [
                "0xfff80000 input_state /* 12-bit ADC value */ eol 2048000 /* X neutral point */ eol sub eol dup sign swap abs eol 100000 /* per-axis deadzone */ eol sub relu eol mul /* restore sign */ eol 2 mul /* scale */",
                "0xfff80001 input_state /* 12-bit ADC value */ eol 2048000 /* Y neutral point */ eol sub eol dup sign swap abs eol 100000 /* per-axis deadzone */ eol sub relu eol mul /* restore sign */ eol -2 mul /* scale */",
                "",
                "",
                "",
                "",
                "",
                ""
            ],
            "quirks": []
        }
    },
    {
        'description': 'custom board v8: analog stick with auto-calibration and proper deadzone',
        'config': {
            "version": 15,
            "unmapped_passthrough_layers": [],
            "partial_scroll_timeout": 1000000,
            "tap_hold_threshold": 200000,
            "gpio_debounce_time_ms": 5,
            "interval_override": 0,
            "our_descriptor_number": 5,
            "ignore_auth_dev_inputs": false,
            "macro_entry_duration": 1,
            "gpio_output_mode": 0,
            "mappings": [
                {
                    "target_usage": "0x00010030",
                    "source_usage": "0xfff50015",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00010031",
                    "source_usage": "0xfff50016",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                }
            ],
            "macros": [
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                []
            ],
            "expressions": [
                "/*\nregister 1 = is neutral point calibration done?\nregister 2 = X neutral point\nregister 3 = Y neutral point\nregister 10 = outer deadzone\nregister 11 = inner deadzone radius\nregister 12 = positive X axis scaling\nregister 13 = positive Y axis scaling\nregister 14 = negative X axis scaling\nregister 15 = negative Y axis scaling\nregister 21 = output stick X\nregister 22 = output stick Y\nregister 23 = processed X value, before applying deadzone\nregister 24 = processed Y value, before applying deadzone\nregister 25 = current stick angle\n*/ eol eol 16000 /* inner deadzone radius */ eol 11000 store eol 16000 /* outer deadzone */ eol 10000 store eol eol /* initial scaling values */ eol 12000 recall dup not 500 mul add 12000 store eol 13000 recall dup not 500 mul add 13000 store eol 14000 recall dup not 500 mul add 14000 store eol 15000 recall dup not 500 mul add 15000 store eol eol /* calibrate neutral point during the first second */ eol 1000 recall eol time 1000000 gt eol bitwise_or eol 1000 store eol eol /* X neutral point */ eol 2000 recall 900 mul eol 0xfff80000 input_state 100 mul eol add eol 1000 recall not mul eol 2000 recall 1000 recall mul eol add eol 2000 store eol eol /* Y neutral point */ eol 3000 recall 900 mul eol 0xfff80001 input_state 100 mul eol add eol 1000 recall not mul eol 3000 recall 1000 recall mul eol add eol 3000 store eol eol /* fetch and process raw values */ eol 0xfff80000 input_state /* 12-bit ADC value for X */ eol 2000 recall -1000 mul add /* X neutral point */ eol 32000 store eol 32000 recall 0 gt 12000 recall mul /* positive side scaling */ eol 32000 recall 0 gt not 14000 recall mul /* negative side scaling */ eol add eol 32000 recall eol mul /* scale */ eol 23000 store /* processed X value */ eol eol 0xfff80001 input_state /* 12-bit ADC value for Y */ eol 3000 recall -1000 mul add /* Y neutral point */ eol -1000 mul /* invert Y axis */ eol 32000 store eol 32000 recall 0 gt 13000 recall mul /* positive side scaling */ eol 32000 recall 0 gt not 15000 recall mul /* negative side scaling */ eol add eol 32000 recall eol mul /* scale */ eol 24000 store /* processed Y value */ eol eol 23000 recall eol 24000 recall eol atan2 eol 25000 store /* angle */ eol eol /* X */ eol 23000 recall /* processed X value */ eol dup sign swap abs eol 25000 recall sin 11000 recall mul /* X component of deadzone */ eol abs sub relu /* apply deadzone */ eol mul /* restore sign */ eol 128000 add /* 128 is neutral on output */ eol 32000 store eol eol /* check if we need to decrease scaling factors */ eol /* negative side */ eol 10000 recall -1000 mul 32000 recall gt eol -1 mul eol 1000 recall mul /* skip if we're still calibrating neutral point */ eol 14000 recall eol add eol 0 99000 clamp 14000 store eol eol /* positive side */ eol 32000 recall 10000 recall 255000 add gt eol -1 mul eol 1000 recall mul /* skip if we're still calibrating neutral point */ eol 12000 recall eol add eol 0 99000 clamp 12000 store eol eol 32000 recall eol 0 255000 clamp /* output range is 0-255 */ eol eol 21000 store eol eol /* Y */ eol eol 24000 recall /* processed Y value */ eol dup sign swap abs eol 25000 recall cos 11000 recall mul /* Y component of deadzone */ eol abs sub relu /* apply deadzone */ eol mul /* restore sign */ eol 128000 add /* 128 is neutral on output */ eol 32000 store eol eol /* check if we need to decrease scaling factors */ eol /* negative side */ eol 10000 recall -1000 mul 32000 recall gt eol -1 mul eol 1000 recall mul /* skip if we're still calibrating neutral point */ eol 15000 recall eol add eol 0 99000 clamp 15000 store eol eol /* positive side */ eol 32000 recall 10000 recall 255000 add gt eol -1 mul eol 1000 recall mul /* skip if we're still calibrating neutral point */ eol 13000 recall eol add eol 0 99000 clamp 13000 store eol eol 32000 recall eol 0 255000 clamp /* output range is 0-255 */ eol eol 22000 store",
                "",
                "",
                "",
                "",
                "",
                "",
                ""
            ],
            "quirks": []
        }
    },
    {
        'description': 'custom usages for DualSense: touchpad works as mouse',
        'config': {
            "version": 12,
            "unmapped_passthrough_layers": [],
            "partial_scroll_timeout": 1000000,
            "tap_hold_threshold": 200000,
            "gpio_debounce_time_ms": 5,
            "interval_override": 0,
            "our_descriptor_number": 0,
            "ignore_auth_dev_inputs": false,
            "macro_entry_duration": 1,
            "gpio_output_mode": 0,
            "mappings": [
                {
                    "target_usage": "0x00010030",
                    "source_usage": "0xfff30001",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00010031",
                    "source_usage": "0xfff30002",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090001",
                    "source_usage": "0x0009000e",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                }
            ],
            "macros": [
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                []
            ],
            "expressions": [
                "0xfffa0003 prev_input_state -1000 mul eol 0xfffa0003 input_state eol add eol 0xfffa0002 input_state_binary not eol mul eol 0xfffa0002 prev_input_state_binary not eol mul eol 0xfffa0001 input_state eol 0xfffa0001 prev_input_state eol eq mul eol eol 0xfffa0007 prev_input_state -1000 mul eol 0xfffa0007 input_state eol add eol 0xfffa0006 input_state_binary not eol mul eol 0xfffa0006 prev_input_state_binary not eol mul eol 0xfffa0005 input_state eol 0xfffa0005 prev_input_state eol eq mul eol eol add",
                "0xfffa0004 prev_input_state -1000 mul eol 0xfffa0004 input_state eol add eol 0xfffa0002 input_state_binary not eol mul eol 0xfffa0002 prev_input_state_binary not eol mul eol 0xfffa0001 input_state eol 0xfffa0001 prev_input_state eol eq mul eol eol 0xfffa0008 prev_input_state -1000 mul eol 0xfffa0008 input_state eol add eol 0xfffa0006 input_state_binary not eol mul eol 0xfffa0006 prev_input_state_binary not eol mul eol 0xfffa0005 input_state eol 0xfffa0005 prev_input_state eol eq mul eol eol add",
                "",
                "",
                "",
                "",
                "",
                ""
            ],
            "quirks": [
                {
                    "vendor_id": "0x054c",
                    "product_id": "0x0ce6",
                    "interface": 3,
                    "report_id": 1,
                    "usage": "0xff000020",
                    "bitpos": 0,
                    "size": 0,
                    "relative": false,
                    "signed": false
                },
                {
                    "vendor_id": "0x054c",
                    "product_id": "0x0ce6",
                    "interface": 3,
                    "report_id": 1,
                    "usage": "0xff000022",
                    "bitpos": 0,
                    "size": 0,
                    "relative": false,
                    "signed": false
                },
                {
                    "vendor_id": "0x054c",
                    "product_id": "0x0ce6",
                    "interface": 3,
                    "report_id": 1,
                    "usage": "0xfffa0001",
                    "bitpos": 256,
                    "size": 7,
                    "relative": false,
                    "signed": false
                },
                {
                    "vendor_id": "0x054c",
                    "product_id": "0x0ce6",
                    "interface": 3,
                    "report_id": 1,
                    "usage": "0xfffa0002",
                    "bitpos": 263,
                    "size": 1,
                    "relative": false,
                    "signed": false
                },
                {
                    "vendor_id": "0x054c",
                    "product_id": "0x0ce6",
                    "interface": 3,
                    "report_id": 1,
                    "usage": "0xfffa0003",
                    "bitpos": 264,
                    "size": 12,
                    "relative": false,
                    "signed": false
                },
                {
                    "vendor_id": "0x054c",
                    "product_id": "0x0ce6",
                    "interface": 3,
                    "report_id": 1,
                    "usage": "0xfffa0004",
                    "bitpos": 276,
                    "size": 12,
                    "relative": false,
                    "signed": false
                },
                {
                    "vendor_id": "0x054c",
                    "product_id": "0x0ce6",
                    "interface": 3,
                    "report_id": 1,
                    "usage": "0xfffa0005",
                    "bitpos": 288,
                    "size": 7,
                    "relative": false,
                    "signed": false
                },
                {
                    "vendor_id": "0x054c",
                    "product_id": "0x0ce6",
                    "interface": 3,
                    "report_id": 1,
                    "usage": "0xfffa0006",
                    "bitpos": 295,
                    "size": 1,
                    "relative": false,
                    "signed": false
                },
                {
                    "vendor_id": "0x054c",
                    "product_id": "0x0ce6",
                    "interface": 3,
                    "report_id": 1,
                    "usage": "0xfffa0007",
                    "bitpos": 296,
                    "size": 12,
                    "relative": false,
                    "signed": false
                },
                {
                    "vendor_id": "0x054c",
                    "product_id": "0x0ce6",
                    "interface": 3,
                    "report_id": 1,
                    "usage": "0xfffa0008",
                    "bitpos": 308,
                    "size": 12,
                    "relative": false,
                    "signed": false
                },
                {
                    "vendor_id": "0x054c",
                    "product_id": "0x0ce6",
                    "interface": 3,
                    "report_id": 1,
                    "usage": "0xfffa0009",
                    "bitpos": 120,
                    "size": 16,
                    "relative": false,
                    "signed": true
                },
                {
                    "vendor_id": "0x054c",
                    "product_id": "0x0ce6",
                    "interface": 3,
                    "report_id": 1,
                    "usage": "0xfffa000a",
                    "bitpos": 136,
                    "size": 16,
                    "relative": false,
                    "signed": true
                },
                {
                    "vendor_id": "0x054c",
                    "product_id": "0x0ce6",
                    "interface": 3,
                    "report_id": 1,
                    "usage": "0xfffa000b",
                    "bitpos": 152,
                    "size": 16,
                    "relative": false,
                    "signed": true
                },
                {
                    "vendor_id": "0x054c",
                    "product_id": "0x0ce6",
                    "interface": 3,
                    "report_id": 1,
                    "usage": "0xfffa000c",
                    "bitpos": 168,
                    "size": 16,
                    "relative": false,
                    "signed": true
                },
                {
                    "vendor_id": "0x054c",
                    "product_id": "0x0ce6",
                    "interface": 3,
                    "report_id": 1,
                    "usage": "0xfffa000d",
                    "bitpos": 184,
                    "size": 16,
                    "relative": false,
                    "signed": true
                },
                {
                    "vendor_id": "0x054c",
                    "product_id": "0x0ce6",
                    "interface": 3,
                    "report_id": 1,
                    "usage": "0xfffa000e",
                    "bitpos": 200,
                    "size": 16,
                    "relative": false,
                    "signed": true
                }
            ]
        }
    },
    {
        'description': 'DualSense gyro mouse',
        'config': {
            "version": 13,
            "unmapped_passthrough_layers": [],
            "partial_scroll_timeout": 1000000,
            "tap_hold_threshold": 200000,
            "gpio_debounce_time_ms": 5,
            "interval_override": 0,
            "our_descriptor_number": 1,
            "ignore_auth_dev_inputs": false,
            "macro_entry_duration": 20,
            "gpio_output_mode": 0,
            "mappings": [
                {
                    "target_usage": "0x00010030",
                    "source_usage": "0xfff50001",
                    "scaling": 1000,
                    "layers": [
                        0,
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00010031",
                    "source_usage": "0xfff50002",
                    "scaling": 1000,
                    "layers": [
                        0,
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090001",
                    "source_usage": "0x00090002",
                    "source_name": "Cross",
                    "scaling": 1000,
                    "layers": [
                        0,
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090002",
                    "source_usage": "0x00090003",
                    "source_name": "Circle",
                    "scaling": 1000,
                    "layers": [
                        0,
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090001",
                    "source_usage": "0x00090006",
                    "source_name": "R1",
                    "scaling": 1000,
                    "layers": [
                        0,
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090002",
                    "source_usage": "0x00090005",
                    "source_name": "L1",
                    "scaling": 1000,
                    "layers": [
                        0,
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0xfff20001",
                    "source_usage": "0x00090001",
                    "source_name": "Square",
                    "scaling": 1000,
                    "layers": [
                        0,
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00010038",
                    "source_usage": "0xfff30003",
                    "scaling": 1000,
                    "layers": [
                        0,
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0xfff10001",
                    "source_usage": "0x0009000d",
                    "source_name": "PS",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": true,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                }
            ],
            "macros": [
                [
                    [
                        "0x00090001"
                    ],
                    [],
                    [
                        "0x00090001"
                    ]
                ],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                []
            ],
            "expressions": [
                "0xfffa000a input_state /* gyro second axis */ eol -32 mul eol /* don't move if R1 pressed to avoid accidental drags */ eol 0x00090006 input_state_binary not mul eol /* don't move if not on layer 0 */ eol layer_state 1 eq mul eol 0x00010030 input_state /* left stick X */ eol -128000 add /* recenter around zero */ eol dup abs 20000 gt mul /* deadzone */ eol 135 mul eol add eol 1000 recall /* absolute cursor position X */ eol add eol 0 32767000 clamp eol 0x00090004 input_state_binary not mul eol 16384000 eol 0x00090004 input_state_binary mul /* triangle recenters cursor */ eol add eol 1000 store /* used as output absolute mouse X */",
                "0xfffa0009 input_state /* gyro first axis */ eol -32 mul eol /* don't move if R1 pressed to avoid accidental drags */ eol 0x00090006 input_state_binary not mul eol /* don't move if not on layer 0 */ eol layer_state 1 eq mul eol 0x00010031 input_state /* left stick Y */ eol -128000 add /* recenter around zero */ eol dup abs 20000 gt mul /* deadzone */ eol 240 mul eol add eol 2000 recall /* absolute cursor position Y */ eol add eol 0 32767000 clamp eol 0x00090004 input_state_binary not mul eol 16384000 eol 0x00090004 input_state_binary mul /* triangle recenters cursor */ eol add eol 2000 store /* used as output absolute mouse Y */",
                "0x00010035 input_state /* right stick Y */ eol -128000 add /* recenter around zero */ eol dup eol abs 20000 gt mul /* deadzone */ eol -1 mul /* scale */ eol 200 mul eol /* result used for vertical scroll */",
                "",
                "",
                "",
                "",
                ""
            ],
            "quirks": [
                {
                    "vendor_id": "0x054c",
                    "product_id": "0x0ce6",
                    "interface": 3,
                    "report_id": 1,
                    "usage": "0xff000020",
                    "bitpos": 0,
                    "size": 0,
                    "relative": false,
                    "signed": false
                },
                {
                    "vendor_id": "0x054c",
                    "product_id": "0x0ce6",
                    "interface": 3,
                    "report_id": 1,
                    "usage": "0xff000022",
                    "bitpos": 0,
                    "size": 0,
                    "relative": false,
                    "signed": false
                },
                {
                    "vendor_id": "0x054c",
                    "product_id": "0x0ce6",
                    "interface": 3,
                    "report_id": 1,
                    "usage": "0xfffa0001",
                    "bitpos": 256,
                    "size": 7,
                    "relative": false,
                    "signed": false
                },
                {
                    "vendor_id": "0x054c",
                    "product_id": "0x0ce6",
                    "interface": 3,
                    "report_id": 1,
                    "usage": "0xfffa0002",
                    "bitpos": 263,
                    "size": 1,
                    "relative": false,
                    "signed": false
                },
                {
                    "vendor_id": "0x054c",
                    "product_id": "0x0ce6",
                    "interface": 3,
                    "report_id": 1,
                    "usage": "0xfffa0003",
                    "bitpos": 264,
                    "size": 12,
                    "relative": false,
                    "signed": false
                },
                {
                    "vendor_id": "0x054c",
                    "product_id": "0x0ce6",
                    "interface": 3,
                    "report_id": 1,
                    "usage": "0xfffa0004",
                    "bitpos": 276,
                    "size": 12,
                    "relative": false,
                    "signed": false
                },
                {
                    "vendor_id": "0x054c",
                    "product_id": "0x0ce6",
                    "interface": 3,
                    "report_id": 1,
                    "usage": "0xfffa0005",
                    "bitpos": 288,
                    "size": 7,
                    "relative": false,
                    "signed": false
                },
                {
                    "vendor_id": "0x054c",
                    "product_id": "0x0ce6",
                    "interface": 3,
                    "report_id": 1,
                    "usage": "0xfffa0006",
                    "bitpos": 295,
                    "size": 1,
                    "relative": false,
                    "signed": false
                },
                {
                    "vendor_id": "0x054c",
                    "product_id": "0x0ce6",
                    "interface": 3,
                    "report_id": 1,
                    "usage": "0xfffa0007",
                    "bitpos": 296,
                    "size": 12,
                    "relative": false,
                    "signed": false
                },
                {
                    "vendor_id": "0x054c",
                    "product_id": "0x0ce6",
                    "interface": 3,
                    "report_id": 1,
                    "usage": "0xfffa0008",
                    "bitpos": 308,
                    "size": 12,
                    "relative": false,
                    "signed": false
                },
                {
                    "vendor_id": "0x054c",
                    "product_id": "0x0ce6",
                    "interface": 3,
                    "report_id": 1,
                    "usage": "0xfffa0009",
                    "bitpos": 120,
                    "size": 16,
                    "relative": false,
                    "signed": true
                },
                {
                    "vendor_id": "0x054c",
                    "product_id": "0x0ce6",
                    "interface": 3,
                    "report_id": 1,
                    "usage": "0xfffa000a",
                    "bitpos": 136,
                    "size": 16,
                    "relative": false,
                    "signed": true
                },
                {
                    "vendor_id": "0x054c",
                    "product_id": "0x0ce6",
                    "interface": 3,
                    "report_id": 1,
                    "usage": "0xfffa000b",
                    "bitpos": 152,
                    "size": 16,
                    "relative": false,
                    "signed": true
                },
                {
                    "vendor_id": "0x054c",
                    "product_id": "0x0ce6",
                    "interface": 3,
                    "report_id": 1,
                    "usage": "0xfffa000c",
                    "bitpos": 168,
                    "size": 16,
                    "relative": false,
                    "signed": true
                },
                {
                    "vendor_id": "0x054c",
                    "product_id": "0x0ce6",
                    "interface": 3,
                    "report_id": 1,
                    "usage": "0xfffa000d",
                    "bitpos": 184,
                    "size": 16,
                    "relative": false,
                    "signed": true
                },
                {
                    "vendor_id": "0x054c",
                    "product_id": "0x0ce6",
                    "interface": 3,
                    "report_id": 1,
                    "usage": "0xfffa000e",
                    "bitpos": 200,
                    "size": 16,
                    "relative": false,
                    "signed": true
                }
            ]
        }
    },
    {
        'description': '5-key chording keyboard (A-S-D-F-space)',
        'config': {
            "version": 13,
            "unmapped_passthrough_layers": [
                1,
                2,
                3,
                4,
                5,
                6,
                7
            ],
            "partial_scroll_timeout": 1000000,
            "tap_hold_threshold": 200000,
            "gpio_debounce_time_ms": 5,
            "interval_override": 0,
            "our_descriptor_number": 0,
            "ignore_auth_dev_inputs": false,
            "macro_entry_duration": 1,
            "gpio_output_mode": 0,
            "mappings": [
                {
                    "source_usage": "0xfff50001",
                    "target_usage": "0x00070004",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff50002",
                    "target_usage": "0x00070005",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff50003",
                    "target_usage": "0x00070006",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff50004",
                    "target_usage": "0x00070007",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff50005",
                    "target_usage": "0x00070008",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff50006",
                    "target_usage": "0x00070009",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff50007",
                    "target_usage": "0x0007000a",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff50008",
                    "target_usage": "0x0007000b",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff50009",
                    "target_usage": "0x0007000c",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff5000a",
                    "target_usage": "0x0007000d",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff5000b",
                    "target_usage": "0x0007000e",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff5000c",
                    "target_usage": "0x0007000f",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff5000d",
                    "target_usage": "0x00070010",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff5000e",
                    "target_usage": "0x00070011",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff5000f",
                    "target_usage": "0x00070012",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff50010",
                    "target_usage": "0x00070013",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff50011",
                    "target_usage": "0x00070014",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff50012",
                    "target_usage": "0x00070015",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff50013",
                    "target_usage": "0x00070016",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff50014",
                    "target_usage": "0x00070017",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff50015",
                    "target_usage": "0x00070018",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff50016",
                    "target_usage": "0x00070019",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff50017",
                    "target_usage": "0x0007001a",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff50018",
                    "target_usage": "0x0007001b",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff50019",
                    "target_usage": "0x0007001c",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff5001a",
                    "target_usage": "0x0007001d",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff5001b",
                    "target_usage": "0x00070036",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff5001c",
                    "target_usage": "0x00070037",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff5001d",
                    "target_usage": "0x00070033",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff5001e",
                    "target_usage": "0x000700e1",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff5001e",
                    "target_usage": "0x00070038",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff5001f",
                    "target_usage": "0x0007002c",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                }
            ],
            "macros": [
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                []
            ],
            "expressions": [
                "/*\nRegister 32 accumulates all the keys that were pressed during\nthe current chord. They are stored on individual bits.\n*/ eol 32000 recall eol 0x0007002c input_state_binary /* space */ eol 1 mul bitwise_or eol 0x00070009 input_state_binary /* F */ eol 2 mul bitwise_or eol 0x00070007 input_state_binary /* D */ eol 4 mul bitwise_or eol 0x00070016 input_state_binary /* S */ eol 8 mul bitwise_or eol 0x00070004 input_state_binary /* A */ eol 16 mul bitwise_or eol 32000 store eol eol /* detect if a chord has just ended */ eol /* check if any of the keys were pressed last frame */ eol 0x0007002c prev_input_state_binary eol 0x00070009 prev_input_state_binary bitwise_or eol 0x00070007 prev_input_state_binary bitwise_or eol 0x00070016 prev_input_state_binary bitwise_or eol 0x00070004 prev_input_state_binary bitwise_or eol /* and if any are pressed this frame */ eol 0x0007002c input_state_binary eol 0x00070009 input_state_binary bitwise_or eol 0x00070007 input_state_binary bitwise_or eol 0x00070016 input_state_binary bitwise_or eol 0x00070004 input_state_binary bitwise_or eol gt /* some keys were pressed last frame but no longer are */ eol /* if so... */ eol dup eol 32000 recall eol mul eol 31000 store /* ... copy value from register 32 to register 31... */ eol not eol 32000 recall mul eol 32000 store /* ... and reset register 32 to zero */ eol eol /* set registers 1-31 according to the key mask */ eol /* they are then used in mappings */ eol 31000 recall 1 eq 1000 store eol 31000 recall 2 eq 2000 store eol 31000 recall 3 eq 3000 store eol 31000 recall 4 eq 4000 store eol 31000 recall 5 eq 5000 store eol 31000 recall 6 eq 6000 store eol 31000 recall 7 eq 7000 store eol 31000 recall 8 eq 8000 store eol 31000 recall 9 eq 9000 store eol 31000 recall 10 eq 10000 store eol 31000 recall 11 eq 11000 store eol 31000 recall 12 eq 12000 store eol 31000 recall 13 eq 13000 store eol 31000 recall 14 eq 14000 store eol 31000 recall 15 eq 15000 store eol 31000 recall 16 eq 16000 store eol 31000 recall 17 eq 17000 store eol 31000 recall 18 eq 18000 store eol 31000 recall 19 eq 19000 store eol 31000 recall 20 eq 20000 store eol 31000 recall 21 eq 21000 store eol 31000 recall 22 eq 22000 store eol 31000 recall 23 eq 23000 store eol 31000 recall 24 eq 24000 store eol 31000 recall 25 eq 25000 store eol 31000 recall 26 eq 26000 store eol 31000 recall 27 eq 27000 store eol 31000 recall 28 eq 28000 store eol 31000 recall 29 eq 29000 store eol 31000 recall 30 eq 30000 store eol /* register 31 was used to store the chord mask */ eol /* but now we re-use it as an output register */ eol 31000 recall 31 eq 31000 store",
                "",
                "",
                "",
                "",
                "",
                "",
                ""
            ],
            "quirks": []
        }
    },
    {
        'description': 'Two mice, two cursors',
        'config': {
            "version": 13,
            "unmapped_passthrough_layers": [
                4,
                5,
                6,
                7
            ],
            "partial_scroll_timeout": 1000000,
            "tap_hold_threshold": 200000,
            "gpio_debounce_time_ms": 5,
            "interval_override": 0,
            "our_descriptor_number": 1,
            "ignore_auth_dev_inputs": false,
            "macro_entry_duration": 1,
            "gpio_output_mode": 0,
            "mappings": [
                {
                    "target_usage": "0x00010030",
                    "source_usage": "0xfff50005",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00010031",
                    "source_usage": "0xfff50006",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0x00090001",
                    "target_usage": "0x00090001",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0x00090002",
                    "target_usage": "0x00090002",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0x00090003",
                    "target_usage": "0x00090003",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                }
            ],
            "macros": [
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                []
            ],
            "expressions": [
                "/*\nregister 1 = mouse 1 absolute cursor position X\nregister 2 = mouse 1 absolute cursor position Y\nregister 3 = mouse 2 absolute cursor position X\nregister 4 = mouse 2 absolute cursor position Y\n*/ eol eol 1000 port 0x00010030 input_state /* mouse 1 X delta */ eol 9000 mul eol 1000 recall /* mouse 1 absolute X position */ eol add eol 0 32767000 clamp eol 1000 store eol eol 1000 port 0x00010031 input_state /* mouse 1 Y delta */ eol 16000 mul eol 2000 recall /* mouse 1 absolute Y position */ eol add eol 0 32767000 clamp eol 2000 store eol eol 2000 port 0x00010030 input_state /* mouse 2 X delta */ eol 9000 mul eol 3000 recall /* mouse 2 absolute X position */ eol add eol 0 32767000 clamp eol 3000 store eol eol 2000 port 0x00010031 input_state /* mouse 2 Y delta */ eol 16000 mul eol 4000 recall /* mouse 2 absolute Y position */ eol add eol 0 32767000 clamp eol 4000 store",
                "/*\nregister 7 = 0 : cursor oscillates between mouse 1 and mouse 2\nregister 7 = 1 : mouse 1 owns the cursor\nregister 7 = 2 : mouse 2 owns the cursor\n*/ eol eol 1000 port 0x00090001 input_state_binary /* mouse 1 left button */ eol 0x00090002 input_state_binary /* mouse 1 right button */ eol bitwise_or eol 0x00090003 input_state_binary /* mouse 1 middle button */ eol bitwise_or eol 7000 store /* set to 1 if any button pressed on mouse 1 */ eol eol 2000 port 0x00090001 input_state_binary /* mouse 2 left button */ eol 0x00090002 input_state_binary /* mouse 2 right button */ eol bitwise_or eol 0x00090003 input_state_binary /* mouse 2 middle button */ eol bitwise_or eol 2000 mul eol 7000 recall not /* check if not already owned by mouse 1 */ eol mul eol 7000 recall eol add eol 7000 store /* set to 2 if any button pressed on mouse 2 */ eol",
                "time 100000 mod 50000 gt /* this is 1 50% of the time */ eol 7000 recall 0 eq /* we're oscillating between the two mice */ eol mul eol 7000 recall 1000 eq /* mouse 1 owns cursor */ eol bitwise_or /* mouse 1 owns cursor or it's their turn */ eol 1000 recall /* mouse 1 absolute X position */ eol mul eol eol time 100000 mod 50000 gt not /* this is 1 the other 50% of the time */ eol 7000 recall 0 eq /* we're oscillating between the two mice */ eol mul eol 7000 recall 2000 eq /* mouse 2 owns cursor */ eol bitwise_or /* mouse 2 owns cursor or it's their turn */ eol 3000 recall /* mouse 2 absolute X position */ eol mul eol eol add eol 5000 store /* used as output mouse X absolute position */ eol eol time 100000 mod 50000 gt /* this is 1 50% of the time */ eol 7000 recall 0 eq /* we're oscillating between the two mice */ eol mul eol 7000 recall 1000 eq /* mouse 1 owns cursor */ eol bitwise_or /* mouse 1 owns cursor or it's their turn */ eol 2000 recall /* mouse 1 absolute Y position */ eol mul eol eol time 100000 mod 50000 gt not /* this is 1 the other 50% of the time */ eol 7000 recall 0 eq /* we're oscillating between the two mice */ eol mul eol 7000 recall 2000 eq /* mouse 2 owns cursor */ eol bitwise_or /* mouse 2 owns cursor or it's their turn */ eol 4000 recall /* mouse 2 absolute Y position */ eol mul eol eol add eol 6000 store /* used as output mouse Y absolute position */",
                "",
                "",
                "",
                "",
                ""
            ],
            "quirks": []
        }
    },
    {
        'description': '"warpd": keyboard-driven, grid-based mouse positioning',
        'config': {
            "version": 13,
            "unmapped_passthrough_layers": [
                0,
                4,
                5,
                6,
                7
            ],
            "partial_scroll_timeout": 1000000,
            "tap_hold_threshold": 200000,
            "gpio_debounce_time_ms": 5,
            "interval_override": 0,
            "our_descriptor_number": 1,
            "ignore_auth_dev_inputs": false,
            "macro_entry_duration": 1,
            "gpio_output_mode": 0,
            "mappings": [
                {
                    "target_usage": "0x00010030",
                    "source_usage": "0xfff50005",
                    "scaling": 1000,
                    "layers": [
                        0,
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00010031",
                    "source_usage": "0xfff50006",
                    "scaling": 1000,
                    "layers": [
                        0,
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff50007",
                    "target_usage": "0xfff10001",
                    "layers": [
                        0,
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0x00070039",
                    "target_usage": "0x00000000",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff50008",
                    "target_usage": "0x00090001",
                    "layers": [
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff50009",
                    "target_usage": "0x00090003",
                    "layers": [
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff5000a",
                    "target_usage": "0x00090002",
                    "layers": [
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff50007",
                    "target_usage": "0x00080002",
                    "layers": [
                        0,
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                }
            ],
            "macros": [
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                []
            ],
            "expressions": [
                "/*\nregister 1: current grid region X lower bound\nregister 2: current grid region X upper bound\nregister 3: current grid region Y lower bound\nregister 4: current grid region Y upper bound\nregister 5: output absolute cursor X\nregister 6: output absolute cursor Y\nregister 7 = 0 : inactive (keyboard works as keyboard)\nregister 7 = 1 : grid mode (UIJK selects quarters)\nregister 7 = 2 : normal mode (HJKL moves cursor directly)\nregister 8: left button\nregister 9: middle button\nregister 10: right button\nregister 11: drag-lock\n*/ eol eol 0x00070039 input_state_binary /* caps lock */ eol 0x00070039 prev_input_state_binary not eol mul eol 32000 store /* caps lock has just been pressed */ eol eol /* if caps lock has just been pressed, reset bounds */ eol eol 0 32000 recall mul /* reset if caps lock just pressed */ eol 1000 recall 32000 recall not mul /* keep current if not */ eol add eol 1000 store /* X lower bound */ eol eol 32767000 32000 recall mul /* reset if caps lock just pressed */ eol 2000 recall 32000 recall not mul /* keep current if not */ eol add eol 2000 store /* X upper bound */ eol eol 0 32000 recall mul /* reset if caps lock just pressed */ eol 3000 recall 32000 recall not mul /* keep current if not */ eol add eol 3000 store /* Y lower bound */ eol eol 32767000 32000 recall mul /* reset if caps lock just pressed */ eol 4000 recall 32000 recall not mul /* keep current if not */ eol add eol 4000 store /* Y upper bound */ eol eol /* if caps lock just pressed set mode to 1=grid */ eol 32000 recall 1000 mul eol 32000 recall not 7000 recall mul /* if not keep mode */ eol add eol 0x00070029 prev_input_state_binary /* esc */ eol 0x00070029 input_state_binary not eol mul /* esc just released */ eol not eol mul /* if esc just released, reset mode to 0=inactive */ eol 7000 store /* mode */ eol eol 11000 recall /* drag-lock state */ eol 0x00070019 prev_input_state_binary /* V */ eol 0x00070019 input_state_binary not eol mul /* V was just released */ eol bitwise_or /* drag-lock was already active or V just released */ eol 11000 recall /* drag-lock state */ eol 0x00070019 prev_input_state_binary /* V */ eol 0x00070019 input_state_binary not eol mul /* V was just released */ eol 0x00070010 prev_input_state_binary /* M */ eol 0x00070010 input_state_binary not eol mul /* M was just released */ eol bitwise_or /* V was just released or M was just released */ eol mul /* and drag-lock is active */ eol not /* so deactivate it */ eol mul eol 7000 recall 0 eq not /* not in inactive mode */ eol mul eol 11000 store /* drag-lock */ eol eol 0x00070010 input_state_binary /* M */ eol 0x00070019 input_state_binary /* V */ eol bitwise_or /* M or V currently pressed */ eol dup eol 11000 recall /* drag-lock state */ eol bitwise_or /* drag-lock active or M or V currently pressed */ eol 8000 store /* used as left mouse button */ eol 0x00070036 input_state_binary /* , */ eol dup eol 9000 store /* used as middle mouse button */ eol bitwise_or /* M or V or \",\" currently pressed */ eol 0x00070037 input_state_binary /* . */ eol dup eol 10000 store /* used as right mouse button */ eol bitwise_or /* M or V or \",\" or \".\" currently pressed */ eol 0x0007000b input_state_binary /* H */ eol bitwise_or eol 0x0007000f input_state_binary /* L */ eol bitwise_or eol 0x00070011 input_state_binary /* N */ eol bitwise_or /* M, V, \",\", \".\", H, L or N currently pressed */ eol 7000 recall 0 eq not /* not in inactive mode */ eol mul eol dup eol 31000 store /* not inactive and one of the keys above pressed */ eol 2000 mul /* set mode to 2=normal (HJKL moves cursor) */ eol 31000 recall not 7000 recall mul /* keep current mode otherwise */ eol add eol 7000 store /* mode */",
                "/* move to top-left quarter */ eol 0x00070018 input_state_binary /* U */ eol 0x00070018 prev_input_state_binary not eol mul eol 30000 store /* U just pressed */ eol eol /* upper_bound = 0.5*(lower_bound+upper_bound) */ eol 1000 recall 2000 recall add eol 500 mul eol 30000 recall mul /* if U just pressed */ eol 2000 recall 30000 recall not mul /* keep current otherwise */ eol add eol 2000 store /* X upper bound */ eol eol /* upper_bound = 0.5*(lower_bound+upper_bound) */ eol 3000 recall 4000 recall add eol 500 mul eol 30000 recall mul /* if U just pressed */ eol 4000 recall 30000 recall not mul /* keep current otherwise */ eol add eol 4000 store /* Y upper bound */ eol eol /* move to top-right quarter */ eol 0x0007000c input_state_binary /* I */ eol 0x0007000c prev_input_state_binary not eol mul eol 29000 store /* I just pressed */ eol eol /* lower_bound = 0.5*(lower_bound+upper_bound) */ eol 1000 recall 2000 recall add eol 500 mul eol 29000 recall mul /* if I just pressed */ eol 1000 recall 29000 recall not mul /* keep current otherwise */ eol add eol 1000 store /* X lower bound */ eol eol /* upper_bound = 0.5*(lower_bound+upper_bound) */ eol 3000 recall 4000 recall add eol 500 mul eol 29000 recall mul /* if I just pressed */ eol 4000 recall 29000 recall not mul /* keep current otherwise */ eol add eol 4000 store /* Y upper bound */ eol eol /* move to bottom-left quarter */ eol 0x0007000d input_state_binary /* J */ eol 0x0007000d prev_input_state_binary not eol mul eol 28000 store /* J just pressed */ eol eol /* upper_bound = 0.5*(lower_bound+upper_bound) */ eol 1000 recall 2000 recall add eol 500 mul eol 28000 recall mul /* if J just pressed */ eol 2000 recall 28000 recall not mul /* keep current otherwise */ eol add eol 2000 store /* X upper bound */ eol eol /* lower_bound = 0.5*(lower_bound+upper_bound) */ eol 3000 recall 4000 recall add eol 500 mul eol 28000 recall mul /* if J just pressed */ eol 3000 recall 28000 recall not mul /* keep current otherwise */ eol add eol 3000 store /* Y lower bound */ eol eol /* move to bottom-right quarter */ eol 0x0007000e input_state_binary /* K */ eol 0x0007000e prev_input_state_binary not eol mul eol 27000 store /* K just pressed */ eol eol /* lower_bound = 0.5*(lower_bound+upper_bound) */ eol 1000 recall 2000 recall add eol 500 mul eol 27000 recall mul /* if K just pressed */ eol 1000 recall 27000 recall not mul /* keep current otherwise */ eol add eol 1000 store /* X lower bound */ eol eol /* lower_bound = 0.5*(lower_bound+upper_bound) */ eol 3000 recall 4000 recall add eol 500 mul eol 27000 recall mul /* if K just pressed */ eol 3000 recall 27000 recall not mul /* keep current otherwise */ eol add eol 3000 store /* Y lower bound */",
                "/* absolute cursor X position */ eol 7000 recall 1000 eq not /* if not grid mode */ eol 5000 recall mul /* start with current position, zero otherwise */ eol eol 7000 recall 2000 eq /* \"normal\" mode - HJKL moves cursor */ eol 0x0007000b input_state_binary /* H */ eol -2250 mul eol 0x0007000f input_state_binary /* L */ eol 2250 mul eol add /* X delta */ eol mul /* use delta if normal mode, zero otherwise */ eol eol add eol eol 7000 recall 1000 eq /* \"grid\" mode - UIJK selects quarters */ eol /* cursor_x = 0.5*(lower_bound+upper_bound) */ eol 1000 recall 2000 recall add eol 500 mul eol mul /* use if we're in grid mode, zero otherwise */ eol eol add eol eol 0 32767000 clamp eol 5000 store /* used as output absolute mouse X */",
                "/* absolute cursor Y position */ eol 7000 recall 1000 eq not /* if not grid mode */ eol 6000 recall mul /* start with current position, zero otherwise */ eol eol 7000 recall 2000 eq /* \"normal\" mode - HJKL moves cursor */ eol 0x0007000d input_state_binary /* J */ eol 4000 mul eol 0x0007000e input_state_binary /* K */ eol -4000 mul eol add /* Y delta */ eol mul /* use delta if normal mode, zero otherwise */ eol eol add eol eol 7000 recall 1000 eq /* \"grid\" mode - UIJK selects quarters */ eol /* cursor_y = 0.5*(lower_bound+upper_bound) */ eol 3000 recall 4000 recall add eol 500 mul eol mul /* use if we're in grid mode, zero otherwise */ eol eol add eol eol 0 32767000 clamp eol 6000 store /* used as output absolute mouse Y */",
                "",
                "",
                "",
                ""
            ],
            "quirks": []
        }
    },
    {
        'description': 'DualSense to XAC adapter',
        'config': {
            "version": 16,
            "unmapped_passthrough_layers": [
                4,
                5,
                6,
                7
            ],
            "partial_scroll_timeout": 1000000,
            "tap_hold_threshold": 200000,
            "gpio_debounce_time_ms": 5,
            "interval_override": 0,
            "our_descriptor_number": 5,
            "ignore_auth_dev_inputs": false,
            "macro_entry_duration": 1,
            "gpio_output_mode": 0,
            "input_labels": 0,
            "mappings": [
                {
                    "target_usage": "0x00010030",
                    "source_usage": "0x00010030",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00010031",
                    "source_usage": "0x00010031",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00010032",
                    "source_usage": "0x00010032",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00010035",
                    "source_usage": "0x00010035",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0xfff90001",
                    "source_usage": "0xfff90001",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0xfff90002",
                    "source_usage": "0xfff90002",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0xfff90003",
                    "source_usage": "0xfff90003",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0xfff90004",
                    "source_usage": "0xfff90004",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090005",
                    "source_usage": "0x00090002",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090006",
                    "source_usage": "0x00090003",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x0009000b",
                    "source_usage": "0x00090001",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x0009000c",
                    "source_usage": "0x00090004",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090004",
                    "source_usage": "0x00090005",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x0009000a",
                    "source_usage": "0x00090006",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090001",
                    "source_usage": "0x00090007",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090002",
                    "source_usage": "0x00090008",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090003",
                    "source_usage": "0x0009000b",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090009",
                    "source_usage": "0x0009000c",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090007",
                    "source_usage": "0x00090009",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090008",
                    "source_usage": "0x0009000a",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                }
            ],
            "macros": [
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                []
            ],
            "expressions": [
                "",
                "",
                "",
                "",
                "",
                "",
                "",
                ""
            ],
            "quirks": [
                {
                    "vendor_id": "0x054c",
                    "product_id": "0x0ce6",
                    "interface": 3,
                    "report_id": 1,
                    "usage": "0xff000020",
                    "bitpos": 0,
                    "size": 0,
                    "relative": false,
                    "signed": false
                },
                {
                    "vendor_id": "0x054c",
                    "product_id": "0x0ce6",
                    "interface": 3,
                    "report_id": 1,
                    "usage": "0xff000022",
                    "bitpos": 0,
                    "size": 0,
                    "relative": false,
                    "signed": false
                },
                {
                    "vendor_id": "0x054c",
                    "product_id": "0x0ce6",
                    "interface": 3,
                    "report_id": 1,
                    "usage": "0xfffa0001",
                    "bitpos": 256,
                    "size": 7,
                    "relative": false,
                    "signed": false
                },
                {
                    "vendor_id": "0x054c",
                    "product_id": "0x0ce6",
                    "interface": 3,
                    "report_id": 1,
                    "usage": "0xfffa0002",
                    "bitpos": 263,
                    "size": 1,
                    "relative": false,
                    "signed": false
                },
                {
                    "vendor_id": "0x054c",
                    "product_id": "0x0ce6",
                    "interface": 3,
                    "report_id": 1,
                    "usage": "0xfffa0003",
                    "bitpos": 264,
                    "size": 12,
                    "relative": false,
                    "signed": false
                },
                {
                    "vendor_id": "0x054c",
                    "product_id": "0x0ce6",
                    "interface": 3,
                    "report_id": 1,
                    "usage": "0xfffa0004",
                    "bitpos": 276,
                    "size": 12,
                    "relative": false,
                    "signed": false
                },
                {
                    "vendor_id": "0x054c",
                    "product_id": "0x0ce6",
                    "interface": 3,
                    "report_id": 1,
                    "usage": "0xfffa0005",
                    "bitpos": 288,
                    "size": 7,
                    "relative": false,
                    "signed": false
                },
                {
                    "vendor_id": "0x054c",
                    "product_id": "0x0ce6",
                    "interface": 3,
                    "report_id": 1,
                    "usage": "0xfffa0006",
                    "bitpos": 295,
                    "size": 1,
                    "relative": false,
                    "signed": false
                },
                {
                    "vendor_id": "0x054c",
                    "product_id": "0x0ce6",
                    "interface": 3,
                    "report_id": 1,
                    "usage": "0xfffa0007",
                    "bitpos": 296,
                    "size": 12,
                    "relative": false,
                    "signed": false
                },
                {
                    "vendor_id": "0x054c",
                    "product_id": "0x0ce6",
                    "interface": 3,
                    "report_id": 1,
                    "usage": "0xfffa0008",
                    "bitpos": 308,
                    "size": 12,
                    "relative": false,
                    "signed": false
                },
                {
                    "vendor_id": "0x054c",
                    "product_id": "0x0ce6",
                    "interface": 3,
                    "report_id": 1,
                    "usage": "0xfffa0009",
                    "bitpos": 120,
                    "size": 16,
                    "relative": false,
                    "signed": true
                },
                {
                    "vendor_id": "0x054c",
                    "product_id": "0x0ce6",
                    "interface": 3,
                    "report_id": 1,
                    "usage": "0xfffa000a",
                    "bitpos": 136,
                    "size": 16,
                    "relative": false,
                    "signed": true
                },
                {
                    "vendor_id": "0x054c",
                    "product_id": "0x0ce6",
                    "interface": 3,
                    "report_id": 1,
                    "usage": "0xfffa000b",
                    "bitpos": 152,
                    "size": 16,
                    "relative": false,
                    "signed": true
                },
                {
                    "vendor_id": "0x054c",
                    "product_id": "0x0ce6",
                    "interface": 3,
                    "report_id": 1,
                    "usage": "0xfffa000c",
                    "bitpos": 168,
                    "size": 16,
                    "relative": false,
                    "signed": true
                },
                {
                    "vendor_id": "0x054c",
                    "product_id": "0x0ce6",
                    "interface": 3,
                    "report_id": 1,
                    "usage": "0xfffa000d",
                    "bitpos": 184,
                    "size": 16,
                    "relative": false,
                    "signed": true
                },
                {
                    "vendor_id": "0x054c",
                    "product_id": "0x0ce6",
                    "interface": 3,
                    "report_id": 1,
                    "usage": "0xfffa000e",
                    "bitpos": 200,
                    "size": 16,
                    "relative": false,
                    "signed": true
                }
            ]
        }
    },
    {
        'description': 'Stadia controller to XAC adapter',
        'config': {
            "version": 16,
            "unmapped_passthrough_layers": [
                4,
                5,
                6,
                7
            ],
            "partial_scroll_timeout": 1000000,
            "tap_hold_threshold": 200000,
            "gpio_debounce_time_ms": 5,
            "interval_override": 0,
            "our_descriptor_number": 5,
            "ignore_auth_dev_inputs": false,
            "macro_entry_duration": 1,
            "gpio_output_mode": 0,
            "input_labels": 0,
            "mappings": [
                {
                    "target_usage": "0x00010030",
                    "source_usage": "0x00010030",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00010031",
                    "source_usage": "0x00010031",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00010032",
                    "source_usage": "0x00010032",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00010035",
                    "source_usage": "0x00010035",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0xfff90001",
                    "source_usage": "0xfff90001",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0xfff90002",
                    "source_usage": "0xfff90002",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0xfff90003",
                    "source_usage": "0xfff90003",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0xfff90004",
                    "source_usage": "0xfff90004",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090005",
                    "source_usage": "0x00090001",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090006",
                    "source_usage": "0x00090002",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x0009000b",
                    "source_usage": "0x00090004",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x0009000c",
                    "source_usage": "0x00090005",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090004",
                    "source_usage": "0x00090007",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x0009000a",
                    "source_usage": "0x00090008",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090001",
                    "source_usage": "0x00090014",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090002",
                    "source_usage": "0x00090013",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090003",
                    "source_usage": "0x0009000e",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090009",
                    "source_usage": "0x0009000f",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090007",
                    "source_usage": "0x0009000b",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090008",
                    "source_usage": "0x0009000c",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                }
            ],
            "macros": [
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                []
            ],
            "expressions": [
                "",
                "",
                "",
                "",
                "",
                "",
                "",
                ""
            ],
            "quirks": []
        }
    },
    {
        'description': 'keyboard to XAC adapter: arrows=D-pad, WASD=left stick, numpad=right stick',
        'config': {
            "version": 16,
            "unmapped_passthrough_layers": [],
            "partial_scroll_timeout": 1000000,
            "tap_hold_threshold": 200000,
            "gpio_debounce_time_ms": 5,
            "interval_override": 0,
            "our_descriptor_number": 5,
            "ignore_auth_dev_inputs": false,
            "macro_entry_duration": 1,
            "gpio_output_mode": 0,
            "input_labels": 0,
            "mappings": [
                {
                    "target_usage": "0xfff90001",
                    "source_usage": "0x00070050",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0xfff90002",
                    "source_usage": "0x0007004f",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0xfff90003",
                    "source_usage": "0x00070052",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0xfff90004",
                    "source_usage": "0x00070051",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00010030",
                    "source_usage": "0xfff30003",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00010031",
                    "source_usage": "0xfff30004",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00010032",
                    "source_usage": "0xfff30006",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00010035",
                    "source_usage": "0xfff30007",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090005",
                    "source_usage": "0x0007000d",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090006",
                    "source_usage": "0x0007000e",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x0009000b",
                    "source_usage": "0x0007000c",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x0009000c",
                    "source_usage": "0x00070012",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x0009000a",
                    "source_usage": "0x00070013",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090002",
                    "source_usage": "0x0007000f",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090004",
                    "source_usage": "0x0007002f",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090001",
                    "source_usage": "0x00070033",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090003",
                    "source_usage": "0x00070009",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090009",
                    "source_usage": "0x0007000b",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090008",
                    "source_usage": "0x00070028",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00090007",
                    "source_usage": "0x0007002a",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                }
            ],
            "macros": [
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                []
            ],
            "expressions": [
                "",
                "0x00070004 input_state_binary /* A */ eol 0x00070007 input_state_binary /* D */ eol 0x0007001a input_state_binary /* W */ eol 0x00070016 input_state_binary /* S */ eol dpad /* 0=up, 1=up+right, 2=right, ..., 8=neutral */ eol 45000 mul /* angle in degrees */ eol dup eol 360000 eq not 3000 store /* is any of the keys pressed? */ eol 4000 store /* 0..360 angle */",
                "4000 recall /* 0..360 angle */ eol sin eol 127000 mul eol 3000 recall /* is any of the keys pressed? */ eol mul eol 128000 add eol /* used as left stick X */",
                "4000 recall /* 0..360 angle */ eol cos eol -127000 eol mul eol 3000 recall /* is any of the keys pressed? */ eol mul eol 128000 add eol /* used as left stick Y */",
                "0x0007005c input_state_binary /* numpad 4 */ eol 0x0007005e input_state_binary /* numpad 6 */ eol 0x00070060 input_state_binary /* numpad 8 */ eol 0x0007005d input_state_binary /* numpad 5 */ eol dpad /* 0=up, 1=up+right, 2=right, ..., 8=neutral */ eol 45000 mul /* angle in degrees */ eol dup eol 360000 eq not 6000 store /* is any of the keys pressed? */ eol 7000 store /* 0..360 angle */",
                "7000 recall /* 0..360 angle */ eol sin eol 127000 mul eol 6000 recall /* is any of the keys pressed? */ eol mul eol 128000 add eol /* used as right stick X */",
                "7000 recall /* 0..360 angle */ eol cos eol -127000 mul eol 6000 recall /* is any of the keys pressed? */ eol mul eol 128000 add eol /* used as right stick Y */",
                ""
            ],
            "quirks": []
        }
    },
    {
        'description': 'scroll wheel text input, left button accepts letter',
        'config': {
            "version": 14,
            "unmapped_passthrough_layers": [],
            "partial_scroll_timeout": 1000000,
            "tap_hold_threshold": 200000,
            "gpio_debounce_time_ms": 5,
            "interval_override": 0,
            "our_descriptor_number": 0,
            "ignore_auth_dev_inputs": false,
            "macro_entry_duration": 1,
            "gpio_output_mode": 0,
            "mappings": [
                {
                    "source_usage": "0xfff50001",
                    "target_usage": "0x00070004",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff50002",
                    "target_usage": "0x00070005",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff50003",
                    "target_usage": "0x00070006",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff50004",
                    "target_usage": "0x00070007",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff50005",
                    "target_usage": "0x00070008",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff50006",
                    "target_usage": "0x00070009",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff50007",
                    "target_usage": "0x0007000a",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff50008",
                    "target_usage": "0x0007000b",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff50009",
                    "target_usage": "0x0007000c",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff5000a",
                    "target_usage": "0x0007000d",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff5000b",
                    "target_usage": "0x0007000e",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff5000c",
                    "target_usage": "0x0007000f",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff5000d",
                    "target_usage": "0x00070010",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff5000e",
                    "target_usage": "0x00070011",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff5000f",
                    "target_usage": "0x00070012",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff50010",
                    "target_usage": "0x00070013",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff50011",
                    "target_usage": "0x00070014",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff50012",
                    "target_usage": "0x00070015",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff50013",
                    "target_usage": "0x00070016",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff50014",
                    "target_usage": "0x00070017",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff50015",
                    "target_usage": "0x00070018",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff50016",
                    "target_usage": "0x00070019",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff50017",
                    "target_usage": "0x0007001a",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff50018",
                    "target_usage": "0x0007001b",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff50019",
                    "target_usage": "0x0007001c",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff5001a",
                    "target_usage": "0x0007001d",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff5001b",
                    "target_usage": "0x0007002c",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff5001c",
                    "target_usage": "0x0007002a",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0x00090003",
                    "target_usage": "0x0007002a",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0x00090002",
                    "target_usage": "0x00070028",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                }
            ],
            "macros": [
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                []
            ],
            "expressions": [
                "/*\nif register 31 is 1, emit currently selected letter,\nwhich is stored in register 32\n*/ eol 32000 recall 0 eq 31000 recall mul 27000 store /* space */ eol 32000 recall 1000 eq 31000 recall mul 1000 store /* A */ eol 32000 recall 2000 eq 31000 recall mul 2000 store /* B */ eol 32000 recall 3000 eq 31000 recall mul 3000 store /* C */ eol 32000 recall 4000 eq 31000 recall mul 4000 store eol 32000 recall 5000 eq 31000 recall mul 5000 store eol 32000 recall 6000 eq 31000 recall mul 6000 store eol 32000 recall 7000 eq 31000 recall mul 7000 store eol 32000 recall 8000 eq 31000 recall mul 8000 store eol 32000 recall 9000 eq 31000 recall mul 9000 store eol 32000 recall 10000 eq 31000 recall mul 10000 store eol 32000 recall 11000 eq 31000 recall mul 11000 store eol 32000 recall 12000 eq 31000 recall mul 12000 store eol 32000 recall 13000 eq 31000 recall mul 13000 store eol 32000 recall 14000 eq 31000 recall mul 14000 store eol 32000 recall 15000 eq 31000 recall mul 15000 store eol 32000 recall 16000 eq 31000 recall mul 16000 store eol 32000 recall 17000 eq 31000 recall mul 17000 store eol 32000 recall 18000 eq 31000 recall mul 18000 store eol 32000 recall 19000 eq 31000 recall mul 19000 store eol 32000 recall 20000 eq 31000 recall mul 20000 store eol 32000 recall 21000 eq 31000 recall mul 21000 store eol 32000 recall 22000 eq 31000 recall mul 22000 store eol 32000 recall 23000 eq 31000 recall mul 23000 store eol 32000 recall 24000 eq 31000 recall mul 24000 store eol 32000 recall 25000 eq 31000 recall mul 25000 store eol 32000 recall 26000 eq 31000 recall mul 26000 store eol eol 32000 recall /* currently selected letter */ eol 0x00010038 input_state /* scroll wheel */ eol add eol 27000 add eol 27000 mod /* wrap around */ eol 0x00090001 input_state_binary not /* left mouse button */ eol mul /* reset if left button (accept letter) pressed */ eol 0x00090002 input_state_binary not /* right mouse button */ eol mul /* reset if right button (enter) pressed */ eol 0x00090003 input_state_binary not /* middle mouse button */ eol mul /* reset if middle button (backspace) pressed */ eol 32000 store /* currently selected letter */ eol eol 0x00010038 input_state 0 eq not /* scroll wheel */ eol 31000 store /* letter pending */ eol eol 0x00010038 input_state 0 eq not /* scroll wheel */ eol 29000 recall /* did we want backspace on next scroll? */ eol mul eol 28000 store /* backspace */ eol eol 29000 recall /* do we want backspace on next scroll */ eol 0x00010038 input_state 0 eq not /* scroll wheel */ eol bitwise_or eol 0x00090001 input_state_binary not /* left mouse button */ eol mul /* no backspace after accepting letter */ eol 0x00090002 input_state_binary not /* right mouse button */ eol mul /* no backspace after enter */ eol 0x00090003 input_state_binary not /* middle mouse button */ eol mul /* no backspace after (explicit) backspace */ eol 29000 store /* do we want backspace on next scroll */",
                "",
                "",
                "",
                "",
                "",
                "",
                ""
            ],
            "quirks": []
        }
    },
    {
        'description': 'keyboard SOCD: neutral',
        'config': {
            "version": 14,
            "unmapped_passthrough_layers": [
                0,
                1,
                2,
                3,
                4,
                5,
                6,
                7
            ],
            "partial_scroll_timeout": 1000000,
            "tap_hold_threshold": 200000,
            "gpio_debounce_time_ms": 5,
            "interval_override": 0,
            "our_descriptor_number": 0,
            "ignore_auth_dev_inputs": false,
            "macro_entry_duration": 1,
            "gpio_output_mode": 0,
            "mappings": [
                {
                    "source_usage": "0xfff30001",
                    "target_usage": "0x00070004",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff30002",
                    "target_usage": "0x00070007",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                }
            ],
            "macros": [
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                []
            ],
            "expressions": [
                "0x00070004 input_state_binary /* A pressed */ eol 0x00070007 input_state_binary not /* D not pressed */ eol mul /* logical AND */ eol /* used as A */",
                "0x00070007 input_state_binary /* D pressed */ eol 0x00070004 input_state_binary not /* A not pressed */ eol mul /* logical AND */ eol /* used as D */",
                "",
                "",
                "",
                "",
                "",
                ""
            ],
            "quirks": []
        }
    },
    {
        'description': 'keyboard SOCD: last input priority',
        'config': {
            "version": 14,
            "unmapped_passthrough_layers": [
                0,
                1,
                2,
                3,
                4,
                5,
                6,
                7
            ],
            "partial_scroll_timeout": 1000000,
            "tap_hold_threshold": 200000,
            "gpio_debounce_time_ms": 5,
            "interval_override": 0,
            "our_descriptor_number": 0,
            "ignore_auth_dev_inputs": false,
            "macro_entry_duration": 1,
            "gpio_output_mode": 0,
            "mappings": [
                {
                    "source_usage": "0xfff30001",
                    "target_usage": "0x00070004",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff30002",
                    "target_usage": "0x00070007",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                }
            ],
            "macros": [
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                []
            ],
            "expressions": [
                "/* register 1 = A is pressed and has priority */ eol 1000 recall eol 0x00070004 prev_input_state_binary not /* A */ eol bitwise_or /* if A was just pressed, it has priority */ eol 0x00070007 input_state_binary not /* D */ eol bitwise_or /* if D is not pressed, A has priority */ eol 0x00070007 prev_input_state_binary not /* D */ eol 0x00070007 input_state_binary /* D */ eol mul eol not eol mul /* if D was just pressed, A doesn't have priority */ eol 0x00070004 input_state_binary /* A */ eol mul /* none of the above matters unless A is pressed */ eol dup eol 1000 store eol /* result used as A */",
                "/* register 2 = D is pressed and has priority */ eol 2000 recall eol 0x00070007 prev_input_state_binary not /* D */ eol bitwise_or /* if D was just pressed, it has priority */ eol 0x00070004 input_state_binary not /* A */ eol bitwise_or /* if A is not pressed, D has priority */ eol 0x00070004 prev_input_state_binary not /* A */ eol 0x00070004 input_state_binary /* A */ eol mul eol not eol mul /* if A was just pressed, D doesn't have priority */ eol 0x00070007 input_state_binary /* D */ eol mul /* none of the above matters unless D is pressed */ eol dup eol 2000 store eol /* result used as D */",
                "",
                "",
                "",
                "",
                "",
                ""
            ],
            "quirks": []
        }
    },
    {
        'description': 'keyboard SOCD: first input priority',
        'config': {
            "version": 14,
            "unmapped_passthrough_layers": [
                0,
                1,
                2,
                3,
                4,
                5,
                6,
                7
            ],
            "partial_scroll_timeout": 1000000,
            "tap_hold_threshold": 200000,
            "gpio_debounce_time_ms": 5,
            "interval_override": 0,
            "our_descriptor_number": 0,
            "ignore_auth_dev_inputs": false,
            "macro_entry_duration": 1,
            "gpio_output_mode": 0,
            "mappings": [
                {
                    "source_usage": "0xfff30001",
                    "target_usage": "0x00070004",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff30002",
                    "target_usage": "0x00070007",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                }
            ],
            "macros": [
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                []
            ],
            "expressions": [
                "/* register 1 = A is pressed and has priority */ eol 1000 recall eol 0x00070007 input_state_binary not /* D */ eol bitwise_or eol 0x00070004 input_state_binary /* A */ eol mul eol dup eol 1000 store eol /* result used as A */",
                "/* register 2 = D is pressed and has priority */ eol 2000 recall eol 0x00070004 input_state_binary not /* A */ eol bitwise_or eol 0x00070007 input_state_binary /* D */ eol mul eol dup eol 2000 store eol /* result used as D */",
                "",
                "",
                "",
                "",
                "",
                ""
            ],
            "quirks": []
        }
    },
    {
        'description': 'one shot sticky modifiers',
        'config': {
            "version": 14,
            "unmapped_passthrough_layers": [
                0,
                1,
                2,
                3,
                4,
                5,
                6,
                7
            ],
            "partial_scroll_timeout": 1000000,
            "tap_hold_threshold": 200000,
            "gpio_debounce_time_ms": 5,
            "interval_override": 0,
            "our_descriptor_number": 0,
            "ignore_auth_dev_inputs": false,
            "macro_entry_duration": 1,
            "gpio_output_mode": 0,
            "mappings": [
                {
                    "source_usage": "0xfff50001",
                    "target_usage": "0x000700e0",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff50002",
                    "target_usage": "0x000700e1",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff50003",
                    "target_usage": "0x000700e2",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff50004",
                    "target_usage": "0x000700e3",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff50005",
                    "target_usage": "0x000700e4",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff50006",
                    "target_usage": "0x000700e5",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff50007",
                    "target_usage": "0x000700e6",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff50008",
                    "target_usage": "0x000700e7",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                }
            ],
            "macros": [
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                []
            ],
            "expressions": [
                "/* register 32 = none of the regular keys has just been released */ eol eol 0 eol 0x00070004 input_state_binary not eol 0x00070004 prev_input_state_binary mul bitwise_or eol 0x00070005 input_state_binary not eol 0x00070005 prev_input_state_binary mul bitwise_or eol 0x00070006 input_state_binary not eol 0x00070006 prev_input_state_binary mul bitwise_or eol 0x00070007 input_state_binary not eol 0x00070007 prev_input_state_binary mul bitwise_or eol 0x00070008 input_state_binary not eol 0x00070008 prev_input_state_binary mul bitwise_or eol 0x00070009 input_state_binary not eol 0x00070009 prev_input_state_binary mul bitwise_or eol 0x0007000a input_state_binary not eol 0x0007000a prev_input_state_binary mul bitwise_or eol 0x0007000b input_state_binary not eol 0x0007000b prev_input_state_binary mul bitwise_or eol 0x0007000c input_state_binary not eol 0x0007000c prev_input_state_binary mul bitwise_or eol 0x0007000d input_state_binary not eol 0x0007000d prev_input_state_binary mul bitwise_or eol 0x0007000e input_state_binary not eol 0x0007000e prev_input_state_binary mul bitwise_or eol 0x0007000f input_state_binary not eol 0x0007000f prev_input_state_binary mul bitwise_or eol 0x00070010 input_state_binary not eol 0x00070010 prev_input_state_binary mul bitwise_or eol 0x00070011 input_state_binary not eol 0x00070011 prev_input_state_binary mul bitwise_or eol 0x00070012 input_state_binary not eol 0x00070012 prev_input_state_binary mul bitwise_or eol 0x00070013 input_state_binary not eol 0x00070013 prev_input_state_binary mul bitwise_or eol 0x00070014 input_state_binary not eol 0x00070014 prev_input_state_binary mul bitwise_or eol 0x00070015 input_state_binary not eol 0x00070015 prev_input_state_binary mul bitwise_or eol 0x00070016 input_state_binary not eol 0x00070016 prev_input_state_binary mul bitwise_or eol 0x00070017 input_state_binary not eol 0x00070017 prev_input_state_binary mul bitwise_or eol 0x00070018 input_state_binary not eol 0x00070018 prev_input_state_binary mul bitwise_or eol 0x00070019 input_state_binary not eol 0x00070019 prev_input_state_binary mul bitwise_or eol 0x0007001a input_state_binary not eol 0x0007001a prev_input_state_binary mul bitwise_or eol 0x0007001b input_state_binary not eol 0x0007001b prev_input_state_binary mul bitwise_or eol 0x0007001c input_state_binary not eol 0x0007001c prev_input_state_binary mul bitwise_or eol 0x0007001d input_state_binary not eol 0x0007001d prev_input_state_binary mul bitwise_or eol 0x0007001e input_state_binary not eol 0x0007001e prev_input_state_binary mul bitwise_or eol 0x0007001f input_state_binary not eol 0x0007001f prev_input_state_binary mul bitwise_or eol 0x00070020 input_state_binary not eol 0x00070020 prev_input_state_binary mul bitwise_or eol 0x00070021 input_state_binary not eol 0x00070021 prev_input_state_binary mul bitwise_or eol 0x00070022 input_state_binary not eol 0x00070022 prev_input_state_binary mul bitwise_or eol 0x00070023 input_state_binary not eol 0x00070023 prev_input_state_binary mul bitwise_or eol 0x00070024 input_state_binary not eol 0x00070024 prev_input_state_binary mul bitwise_or eol 0x00070025 input_state_binary not eol 0x00070025 prev_input_state_binary mul bitwise_or eol 0x00070026 input_state_binary not eol 0x00070026 prev_input_state_binary mul bitwise_or eol 0x00070027 input_state_binary not eol 0x00070027 prev_input_state_binary mul bitwise_or eol 0x00070028 input_state_binary not eol 0x00070028 prev_input_state_binary mul bitwise_or eol 0x00070029 input_state_binary not eol 0x00070029 prev_input_state_binary mul bitwise_or eol 0x0007002a input_state_binary not eol 0x0007002a prev_input_state_binary mul bitwise_or eol 0x0007002b input_state_binary not eol 0x0007002b prev_input_state_binary mul bitwise_or eol 0x0007002c input_state_binary not eol 0x0007002c prev_input_state_binary mul bitwise_or eol 0x00070039 input_state_binary not eol 0x00070039 prev_input_state_binary mul bitwise_or eol 0x00070046 input_state_binary not eol 0x00070046 prev_input_state_binary mul bitwise_or eol 0x00070047 input_state_binary not eol 0x00070047 prev_input_state_binary mul bitwise_or eol 0x00070048 input_state_binary not eol 0x00070048 prev_input_state_binary mul bitwise_or eol 0x00070049 input_state_binary not eol 0x00070049 prev_input_state_binary mul bitwise_or eol 0x0007004a input_state_binary not eol 0x0007004a prev_input_state_binary mul bitwise_or eol 0x0007004b input_state_binary not eol 0x0007004b prev_input_state_binary mul bitwise_or eol 0x0007004c input_state_binary not eol 0x0007004c prev_input_state_binary mul bitwise_or eol 0x0007004d input_state_binary not eol 0x0007004d prev_input_state_binary mul bitwise_or eol 0x0007004e input_state_binary not eol 0x0007004e prev_input_state_binary mul bitwise_or eol 0x0007004f input_state_binary not eol 0x0007004f prev_input_state_binary mul bitwise_or eol 0x00070050 input_state_binary not eol 0x00070050 prev_input_state_binary mul bitwise_or eol 0x00070051 input_state_binary not eol 0x00070051 prev_input_state_binary mul bitwise_or eol 0x00070052 input_state_binary not eol 0x00070052 prev_input_state_binary mul bitwise_or eol 0x00070053 input_state_binary not eol 0x00070053 prev_input_state_binary mul bitwise_or eol 0x0007002d input_state_binary not eol 0x0007002d prev_input_state_binary mul bitwise_or eol 0x0007002e input_state_binary not eol 0x0007002e prev_input_state_binary mul bitwise_or eol 0x0007002f input_state_binary not eol 0x0007002f prev_input_state_binary mul bitwise_or eol 0x00070030 input_state_binary not eol 0x00070030 prev_input_state_binary mul bitwise_or eol 0x00070031 input_state_binary not eol 0x00070031 prev_input_state_binary mul bitwise_or eol 0x00070033 input_state_binary not eol 0x00070033 prev_input_state_binary mul bitwise_or eol 0x00070034 input_state_binary not eol 0x00070034 prev_input_state_binary mul bitwise_or eol 0x00070035 input_state_binary not eol 0x00070035 prev_input_state_binary mul bitwise_or eol 0x00070036 input_state_binary not eol 0x00070036 prev_input_state_binary mul bitwise_or eol 0x00070037 input_state_binary not eol 0x00070037 prev_input_state_binary mul bitwise_or eol 0x00070038 input_state_binary not eol 0x00070038 prev_input_state_binary mul bitwise_or eol 0x0007003a input_state_binary not eol 0x0007003a prev_input_state_binary mul bitwise_or eol 0x0007003b input_state_binary not eol 0x0007003b prev_input_state_binary mul bitwise_or eol 0x0007003c input_state_binary not eol 0x0007003c prev_input_state_binary mul bitwise_or eol 0x0007003d input_state_binary not eol 0x0007003d prev_input_state_binary mul bitwise_or eol 0x0007003e input_state_binary not eol 0x0007003e prev_input_state_binary mul bitwise_or eol 0x0007003f input_state_binary not eol 0x0007003f prev_input_state_binary mul bitwise_or eol 0x00070040 input_state_binary not eol 0x00070040 prev_input_state_binary mul bitwise_or eol 0x00070041 input_state_binary not eol 0x00070041 prev_input_state_binary mul bitwise_or eol 0x00070042 input_state_binary not eol 0x00070042 prev_input_state_binary mul bitwise_or eol 0x00070043 input_state_binary not eol 0x00070043 prev_input_state_binary mul bitwise_or eol 0x00070044 input_state_binary not eol 0x00070044 prev_input_state_binary mul bitwise_or eol 0x00070045 input_state_binary not eol 0x00070045 prev_input_state_binary mul bitwise_or eol 0x00070068 input_state_binary not eol 0x00070068 prev_input_state_binary mul bitwise_or eol 0x00070069 input_state_binary not eol 0x00070069 prev_input_state_binary mul bitwise_or eol 0x0007006a input_state_binary not eol 0x0007006a prev_input_state_binary mul bitwise_or eol 0x0007006b input_state_binary not eol 0x0007006b prev_input_state_binary mul bitwise_or eol 0x0007006c input_state_binary not eol 0x0007006c prev_input_state_binary mul bitwise_or eol 0x0007006d input_state_binary not eol 0x0007006d prev_input_state_binary mul bitwise_or eol 0x0007006e input_state_binary not eol 0x0007006e prev_input_state_binary mul bitwise_or eol 0x0007006f input_state_binary not eol 0x0007006f prev_input_state_binary mul bitwise_or eol 0x00070070 input_state_binary not eol 0x00070070 prev_input_state_binary mul bitwise_or eol 0x00070071 input_state_binary not eol 0x00070071 prev_input_state_binary mul bitwise_or eol 0x00070072 input_state_binary not eol 0x00070072 prev_input_state_binary mul bitwise_or eol 0x00070073 input_state_binary not eol 0x00070073 prev_input_state_binary mul bitwise_or eol 0x00070054 input_state_binary not eol 0x00070054 prev_input_state_binary mul bitwise_or eol 0x00070055 input_state_binary not eol 0x00070055 prev_input_state_binary mul bitwise_or eol 0x00070056 input_state_binary not eol 0x00070056 prev_input_state_binary mul bitwise_or eol 0x00070057 input_state_binary not eol 0x00070057 prev_input_state_binary mul bitwise_or eol 0x00070058 input_state_binary not eol 0x00070058 prev_input_state_binary mul bitwise_or eol 0x00070059 input_state_binary not eol 0x00070059 prev_input_state_binary mul bitwise_or eol 0x0007005a input_state_binary not eol 0x0007005a prev_input_state_binary mul bitwise_or eol 0x0007005b input_state_binary not eol 0x0007005b prev_input_state_binary mul bitwise_or eol 0x0007005c input_state_binary not eol 0x0007005c prev_input_state_binary mul bitwise_or eol 0x0007005d input_state_binary not eol 0x0007005d prev_input_state_binary mul bitwise_or eol 0x0007005e input_state_binary not eol 0x0007005e prev_input_state_binary mul bitwise_or eol 0x0007005f input_state_binary not eol 0x0007005f prev_input_state_binary mul bitwise_or eol 0x00070060 input_state_binary not eol 0x00070060 prev_input_state_binary mul bitwise_or eol 0x00070061 input_state_binary not eol 0x00070061 prev_input_state_binary mul bitwise_or eol 0x00070062 input_state_binary not eol 0x00070062 prev_input_state_binary mul bitwise_or eol 0x00070063 input_state_binary not eol 0x00070063 prev_input_state_binary mul bitwise_or eol 0x00070067 input_state_binary not eol 0x00070067 prev_input_state_binary mul bitwise_or eol 0x00070064 input_state_binary not eol 0x00070064 prev_input_state_binary mul bitwise_or eol 0x00070032 input_state_binary not eol 0x00070032 prev_input_state_binary mul bitwise_or eol 0x00070065 input_state_binary not eol 0x00070065 prev_input_state_binary mul bitwise_or eol 0x00070066 input_state_binary not eol 0x00070066 prev_input_state_binary mul bitwise_or eol 0x00070087 input_state_binary not eol 0x00070087 prev_input_state_binary mul bitwise_or eol 0x00070089 input_state_binary not eol 0x00070089 prev_input_state_binary mul bitwise_or eol 0x0007008a input_state_binary not eol 0x0007008a prev_input_state_binary mul bitwise_or eol 0x0007008b input_state_binary not eol 0x0007008b prev_input_state_binary mul bitwise_or eol 0x00070088 input_state_binary not eol 0x00070088 prev_input_state_binary mul bitwise_or eol 0x00070090 input_state_binary not eol 0x00070090 prev_input_state_binary mul bitwise_or eol 0x00070091 input_state_binary not eol 0x00070091 prev_input_state_binary mul bitwise_or eol not eol 32000 store",
                "/*\nPressing any of the modifier keys latches it,\nbut whenever any regular key is released, all modifiers\nare unlatched.\n*/ eol eol 1000 recall eol 0x000700e0 input_state_binary eol 0x000700e0 prev_input_state_binary not mul bitwise_or eol 32000 recall mul 1000 store eol eol 2000 recall eol 0x000700e1 input_state_binary eol 0x000700e1 prev_input_state_binary not mul bitwise_or eol 32000 recall mul 2000 store eol eol 3000 recall eol 0x000700e2 input_state_binary eol 0x000700e2 prev_input_state_binary not mul bitwise_or eol 32000 recall mul 3000 store eol eol 4000 recall eol 0x000700e3 input_state_binary eol 0x000700e3 prev_input_state_binary not mul bitwise_or eol 32000 recall mul 4000 store eol eol 5000 recall eol 0x000700e4 input_state_binary eol 0x000700e4 prev_input_state_binary not mul bitwise_or eol 32000 recall mul 5000 store eol eol 6000 recall eol 0x000700e5 input_state_binary eol 0x000700e5 prev_input_state_binary not mul bitwise_or eol 32000 recall mul 6000 store eol eol 7000 recall eol 0x000700e6 input_state_binary eol 0x000700e6 prev_input_state_binary not mul bitwise_or eol 32000 recall mul 7000 store eol eol 8000 recall eol 0x000700e7 input_state_binary eol 0x000700e7 prev_input_state_binary not mul bitwise_or eol 32000 recall mul 8000 store",
                "",
                "",
                "",
                "",
                "",
                ""
            ],
            "quirks": []
        }
    },
    {
        'description': 'right button is left button with turbo',
        'config': {
            "version": 14,
            "unmapped_passthrough_layers": [
                0,
                1,
                2,
                3,
                4,
                5,
                6,
                7
            ],
            "partial_scroll_timeout": 1000000,
            "tap_hold_threshold": 200000,
            "gpio_debounce_time_ms": 5,
            "interval_override": 0,
            "our_descriptor_number": 0,
            "ignore_auth_dev_inputs": false,
            "macro_entry_duration": 1,
            "gpio_output_mode": 0,
            "mappings": [
                {
                    "source_usage": "0xfff30001",
                    "target_usage": "0x00090001",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                }
            ],
            "macros": [
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                []
            ],
            "expressions": [
                "0x00090002 input_state_binary /* right button */ eol time 100000 mod 50000 gt /* click 10 times per second */ eol mul",
                "",
                "",
                "",
                "",
                "",
                "",
                ""
            ],
            "quirks": []
        }
    },
    {
        'description': 'Flatbox rev5',
        'config': {
            "version": 16,
            "unmapped_passthrough_layers": [
                0,
                1,
                2,
                3,
                4,
                5,
                6,
                7
            ],
            "partial_scroll_timeout": 1000000,
            "tap_hold_threshold": 200000,
            "gpio_debounce_time_ms": 5,
            "interval_override": 0,
            "our_descriptor_number": 2,
            "ignore_auth_dev_inputs": false,
            "macro_entry_duration": 1,
            "gpio_output_mode": 0,
            "input_labels": 0,
            "mappings": [
                {
                    "source_usage": "0xfff4000d",
                    "target_usage": "0xfff90001",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff4000b",
                    "target_usage": "0xfff90002",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff4000a",
                    "target_usage": "0xfff90003",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff4000c",
                    "target_usage": "0xfff90004",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff40009",
                    "target_usage": "0x00090002",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff40007",
                    "target_usage": "0x00090003",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff40008",
                    "target_usage": "0x00090001",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff40006",
                    "target_usage": "0x00090004",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff40004",
                    "target_usage": "0x00090006",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff40005",
                    "target_usage": "0x00090008",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff40002",
                    "target_usage": "0x00090005",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff40003",
                    "target_usage": "0x00090007",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff4000e",
                    "target_usage": "0x00090009",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff4000f",
                    "target_usage": "0x0009000a",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff4001a",
                    "target_usage": "0x0009000d",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff4001b",
                    "target_usage": "0x0009000b",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff4001c",
                    "target_usage": "0x0009000c",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                }
            ],
            "macros": [
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                []
            ],
            "expressions": [
                "",
                "",
                "",
                "",
                "",
                "",
                "",
                ""
            ],
            "quirks": []
        }
    },
    {
        'description': 'Flatbox rev4/rev8',
        'config': {
            "version": 17,
            "unmapped_passthrough_layers": [
                0,
                1,
                2,
                3,
                4,
                5,
                6,
                7
            ],
            "partial_scroll_timeout": 1000000,
            "tap_hold_threshold": 200000,
            "gpio_debounce_time_ms": 5,
            "interval_override": 0,
            "our_descriptor_number": 2,
            "ignore_auth_dev_inputs": false,
            "macro_entry_duration": 1,
            "gpio_output_mode": 0,
            "input_labels": 0,
            "mappings": [
                {
                    "source_usage": "0xfff40009",
                    "target_usage": "0xfff90001",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff4000b",
                    "target_usage": "0xfff90002",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff40010",
                    "target_usage": "0xfff90003",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff4000a",
                    "target_usage": "0xfff90004",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff40013",
                    "target_usage": "0x00090002",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff40018",
                    "target_usage": "0x00090003",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff40012",
                    "target_usage": "0x00090001",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff40019",
                    "target_usage": "0x00090004",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff4001b",
                    "target_usage": "0x00090006",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff4001a",
                    "target_usage": "0x00090008",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff4001d",
                    "target_usage": "0x00090005",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff4001c",
                    "target_usage": "0x00090007",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff40006",
                    "target_usage": "0x0009000a",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff40005",
                    "target_usage": "0x00090009",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff40004",
                    "target_usage": "0x0009000d",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff40003",
                    "target_usage": "0x0009000e",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff40002",
                    "target_usage": "0x0009000b",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff40001",
                    "target_usage": "0x0009000c",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                }
            ],
            "macros": [
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                []
            ],
            "expressions": [
                "",
                "",
                "",
                "",
                "",
                "",
                "",
                ""
            ],
            "quirks": []
        }
    },
    {
        'description': '3dRudder (PS4 version): analog stick',
        'config': {
            "version": 15,
            "unmapped_passthrough_layers": [
                0,
                1,
                2,
                3,
                4,
                5,
                6,
                7
            ],
            "partial_scroll_timeout": 1000000,
            "tap_hold_threshold": 200000,
            "gpio_debounce_time_ms": 5,
            "interval_override": 0,
            "our_descriptor_number": 2,
            "ignore_auth_dev_inputs": false,
            "macro_entry_duration": 1,
            "gpio_output_mode": 0,
            "mappings": [
                {
                    "source_usage": "0xfff50001",
                    "target_usage": "0x00010030",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff50002",
                    "target_usage": "0x00010031",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                }
            ],
            "macros": [
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                []
            ],
            "expressions": [
                "/*\nIf message type is not 245, ignore report and use previous value.\nIf status is not 6 or 7, set stick to neutral.\n*/ eol eol 0xff000002 input_state 6000 eq eol 0xff000002 input_state 7000 eq bitwise_or eol 3000 store eol eol /* X */ eol 0xff000001 input_state 245000 eq eol eol 0xff000003 input_state_fp32 /* yaw */ eol dup 0xffff0003 monitor /* preview decoded value */ eol 3000 recall mul /* is status 6 or 7? */ eol 12000 /* horizontal scaling */ eol mul eol dup sign swap abs eol 16000 /* horizontal deadzone */ eol sub relu eol mul /* restore sign */ eol 128000 add eol 0 255000 clamp eol eol 1000 recall /* previous value */ eol eol ifte eol 1000 store eol eol /* Y */ eol 0xff000001 input_state 245000 eq eol eol 0xff000004 input_state_fp32 /* pitch */ eol dup 0xffff0004 monitor /* preview decoded value */ eol 3000 recall mul /* is status 6 or 7? */ eol -20000 /* vertical scaling */ eol mul eol dup sign swap abs eol 16000 /* vertical deadzone */ eol sub relu eol mul /* restore sign */ eol 128000 add eol 0 255000 clamp eol eol 2000 recall /* previous value */ eol eol ifte eol 2000 store",
                "",
                "",
                "",
                "",
                "",
                "",
                ""
            ],
            "quirks": [
                {
                    "vendor_id": "0x2dfa",
                    "product_id": "0x0002",
                    "interface": 0,
                    "report_id": 1,
                    "usage": "0xff000001",
                    "size": 8,
                    "bitpos": 8,
                    "relative": false,
                    "signed": false
                },
                {
                    "vendor_id": "0x2dfa",
                    "product_id": "0x0002",
                    "interface": 0,
                    "report_id": 1,
                    "usage": "0xff000002",
                    "size": 8,
                    "bitpos": 24,
                    "relative": false,
                    "signed": false
                },
                {
                    "vendor_id": "0x2dfa",
                    "product_id": "0x0002",
                    "interface": 0,
                    "report_id": 1,
                    "usage": "0xff000003",
                    "size": 32,
                    "bitpos": 40,
                    "relative": false,
                    "signed": false
                },
                {
                    "vendor_id": "0x2dfa",
                    "product_id": "0x0002",
                    "interface": 0,
                    "report_id": 1,
                    "usage": "0xff000004",
                    "size": 32,
                    "bitpos": 72,
                    "relative": false,
                    "signed": false
                },
                {
                    "vendor_id": "0x2dfa",
                    "product_id": "0x0002",
                    "interface": 0,
                    "report_id": 1,
                    "usage": "0xff000005",
                    "size": 32,
                    "bitpos": 104,
                    "relative": false,
                    "signed": false
                }
            ]
        }
    },
    {
        'description': '3dRudder (PS4 version): mouse',
        'config': {
            "version": 15,
            "unmapped_passthrough_layers": [
                0,
                1,
                2,
                3,
                4,
                5,
                6,
                7
            ],
            "partial_scroll_timeout": 1000000,
            "tap_hold_threshold": 200000,
            "gpio_debounce_time_ms": 5,
            "interval_override": 0,
            "our_descriptor_number": 0,
            "ignore_auth_dev_inputs": false,
            "macro_entry_duration": 1,
            "gpio_output_mode": 0,
            "mappings": [
                {
                    "source_usage": "0xfff30001",
                    "target_usage": "0x00010030",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 100,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff30002",
                    "target_usage": "0x00010031",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 100,
                    "source_port": 0,
                    "target_port": 0
                }
            ],
            "macros": [
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                []
            ],
            "expressions": [
                "/* X */ eol 0xff000003 input_state_fp32 /* yaw */ eol dup 0xffff0003 monitor /* preview decoded value */ eol dup sign swap eol abs 4000 sub relu /* deadzone */ eol mul /* restore sign */ eol /* ignore unless message type is 245 and status is 6 or 7 */ eol 0xff000001 input_state 245000 eq mul eol 0xff000002 input_state 6000 eq eol 0xff000002 input_state 7000 eq bitwise_or mul",
                "/* Y */ eol 0xff000004 input_state_fp32 /* pitch */ eol dup 0xffff0004 monitor /* preview decoded value */ eol dup sign swap eol abs 4000 sub relu /* deadzone */ eol mul /* restore sign */ eol /* ignore unless message type is 245 and status is 6 or 7 */ eol 0xff000001 input_state 245000 eq mul eol 0xff000002 input_state 6000 eq eol 0xff000002 input_state 7000 eq bitwise_or mul",
                "",
                "",
                "",
                "",
                "",
                ""
            ],
            "quirks": [
                {
                    "vendor_id": "0x2dfa",
                    "product_id": "0x0002",
                    "interface": 0,
                    "report_id": 1,
                    "usage": "0xff000001",
                    "size": 8,
                    "bitpos": 8,
                    "relative": false,
                    "signed": false
                },
                {
                    "vendor_id": "0x2dfa",
                    "product_id": "0x0002",
                    "interface": 0,
                    "report_id": 1,
                    "usage": "0xff000002",
                    "size": 8,
                    "bitpos": 24,
                    "relative": false,
                    "signed": false
                },
                {
                    "vendor_id": "0x2dfa",
                    "product_id": "0x0002",
                    "interface": 0,
                    "report_id": 1,
                    "usage": "0xff000003",
                    "size": 32,
                    "bitpos": 40,
                    "relative": false,
                    "signed": false
                },
                {
                    "vendor_id": "0x2dfa",
                    "product_id": "0x0002",
                    "interface": 0,
                    "report_id": 1,
                    "usage": "0xff000004",
                    "size": 32,
                    "bitpos": 72,
                    "relative": false,
                    "signed": false
                },
                {
                    "vendor_id": "0x2dfa",
                    "product_id": "0x0002",
                    "interface": 0,
                    "report_id": 1,
                    "usage": "0xff000005",
                    "size": 32,
                    "bitpos": 104,
                    "relative": false,
                    "signed": false
                }
            ]
        }
    },
    {
        'description': 'touchpad as analog sticks on a PS4 fightstick',
        'config': {
            "version": 17,
            "unmapped_passthrough_layers": [
                0,
                1,
                2,
                3,
                4,
                5,
                6,
                7
            ],
            "partial_scroll_timeout": 1000000,
            "tap_hold_threshold": 200000,
            "gpio_debounce_time_ms": 5,
            "interval_override": 0,
            "our_descriptor_number": 3,
            "ignore_auth_dev_inputs": false,
            "macro_entry_duration": 1,
            "gpio_output_mode": 0,
            "input_labels": 1,
            "mappings": [
                {
                    "target_usage": "0x00010030",
                    "source_usage": "0xfff50001",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00010031",
                    "source_usage": "0xfff50002",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00010032",
                    "source_usage": "0xfff50003",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x00010035",
                    "source_usage": "0xfff50004",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x0009000b",
                    "source_usage": "0xfff50009",
                    "scaling": 1000,
                    "layers": [],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "target_usage": "0x0009000c",
                    "source_usage": "0xfff5000a",
                    "scaling": 1000,
                    "layers": [],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "source_port": 0,
                    "target_port": 0
                }
            ],
            "macros": [
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                []
            ],
            "expressions": [
                "600 11000 store /* X sensitivity */ eol 600 12000 store /* Y sensitivity */ eol eol /* first finger=left stick? */ eol 0xfffa0002 prev_input_state_binary eol 0xfffa0002 input_state_binary not eol mul /* first finger started touching */ eol 960000 0xfffa0003 input_state gt mul /* on left side */ eol dup eol 0xfffa0003 input_state eol 13000 recall ifte 13000 store /* left stick center X */ eol dup eol 0xfffa0004 input_state eol 14000 recall ifte 14000 store /* left stick center Y */ eol 5000 recall bitwise_or eol 0xfffa0002 input_state_binary not mul eol 5000 store /* first finger controls left stick */ eol eol /* first finger=right stick? */ eol 0xfffa0002 prev_input_state_binary eol 0xfffa0002 input_state_binary not eol mul /* first finger started touching */ eol 0xfffa0003 input_state 960000 gt mul /* on right side */ eol dup eol 0xfffa0003 input_state eol 15000 recall ifte 15000 store /* right stick center X */ eol dup eol 0xfffa0004 input_state eol 16000 recall ifte 16000 store /* right stick center Y */ eol 6000 recall bitwise_or eol 0xfffa0002 input_state_binary not mul eol 6000 store /* first finger controls right stick */ eol eol /* second finger=left stick? */ eol 0xfffa0006 prev_input_state_binary eol 0xfffa0006 input_state_binary not eol mul /* second finger started touching */ eol 960000 0xfffa0007 input_state gt mul /* on left side */ eol dup eol 0xfffa0007 input_state eol 13000 recall ifte 13000 store /* left stick center X */ eol dup eol 0xfffa0008 input_state eol 14000 recall ifte 14000 store /* left stick center Y */ eol 7000 recall bitwise_or eol 0xfffa0006 input_state_binary not mul eol 7000 store /* second finger controls left stick */ eol eol /* second finger=right stick? */ eol 0xfffa0006 prev_input_state_binary eol 0xfffa0006 input_state_binary not eol mul /* second finger started touching */ eol 0xfffa0007 input_state 960000 gt mul /* on right side */ eol dup eol 0xfffa0007 input_state eol 15000 recall ifte 15000 store /* right stick center X */ eol dup eol 0xfffa0008 input_state eol 16000 recall ifte 16000 store /* right stick center Y */ eol 8000 recall bitwise_or eol 0xfffa0006 input_state_binary not mul eol 8000 store /* second finger controls right stick */ eol eol 0xfffa0003 input_state /* first finger X */ eol 13000 recall -1000 mul add /* left stick center X */ eol 5000 recall mul /* first finger controls left stick? */ eol 0xfffa0007 input_state /* second finger X */ eol 13000 recall -1000 mul add /* left stick center X */ eol 7000 recall mul /* second finger controls left stick? */ eol add eol 11000 recall mul /* X sensitivity */ eol 128000 add 0 255000 clamp 1000 store /* left stick X */ eol eol 0xfffa0004 input_state /* first finger Y */ eol 14000 recall -1000 mul add /* left stick center Y */ eol 5000 recall mul /* first finger controls left stick? */ eol 0xfffa0008 input_state /* second finger Y */ eol 14000 recall -1000 mul add /* left stick center Y */ eol 7000 recall mul /* second finger controls left stick? */ eol add eol 12000 recall mul /* Y sensitivity */ eol 128000 add 0 255000 clamp 2000 store /* left stick Y */ eol eol 0xfffa0003 input_state /* first finger X */ eol 15000 recall -1000 mul add /* right stick center X */ eol 6000 recall mul /* first finger controls right stick? */ eol 0xfffa0007 input_state /* second finger X */ eol 15000 recall -1000 mul add /* right stick center X */ eol 8000 recall mul /* second finger controls right stick? */ eol add eol 11000 recall mul /* X sensitivity */ eol 128000 add 0 255000 clamp 3000 store /* right stick X */ eol eol 0xfffa0004 input_state /* first finger Y */ eol 16000 recall -1000 mul add /* right stick center Y */ eol 6000 recall mul /* first finger controls right stick? */ eol 0xfffa0008 input_state /* second finger Y */ eol 16000 recall -1000 mul add /* right stick center Y */ eol 8000 recall mul /* second finger controls right stick? */ eol add eol 12000 recall mul /* Y sensitivity */ eol 128000 add 0 255000 clamp 4000 store /* right stick Y */ eol eol 0x0009000e input_state_binary /* touchpad click */ eol 5000 recall /* first finger touching on left side? */ eol 7000 recall bitwise_or /* second finger touching on left side? */ eol mul eol 6000 recall not mul /* first finger NOT touching on right side */ eol 8000 recall not mul /* second finger NOT touching on right side */ eol 9000 store /* L3 */ eol eol 0x0009000e input_state_binary /* touchpad click */ eol 6000 recall /* first finger touching on right side? */ eol 8000 recall bitwise_or /* second finger touching on right side? */ eol mul eol 5000 recall not mul /* first finger NOT touching on left side */ eol 7000 recall not mul /* second finger NOT touching on left side */ eol 10000 store /* R3 */",
                "",
                "",
                "",
                "",
                "",
                "",
                ""
            ],
            "quirks": [
                {
                    "vendor_id": "0x0000",
                    "product_id": "0x0000",
                    "interface": 0,
                    "report_id": 1,
                    "usage": "0xfffa0001",
                    "bitpos": 272,
                    "size": 7,
                    "relative": false,
                    "signed": false
                },
                {
                    "vendor_id": "0x0000",
                    "product_id": "0x0000",
                    "interface": 0,
                    "report_id": 1,
                    "usage": "0xfffa0002",
                    "bitpos": 279,
                    "size": 1,
                    "relative": false,
                    "signed": false
                },
                {
                    "vendor_id": "0x0000",
                    "product_id": "0x0000",
                    "interface": 0,
                    "report_id": 1,
                    "usage": "0xfffa0003",
                    "bitpos": 280,
                    "size": 12,
                    "relative": false,
                    "signed": false
                },
                {
                    "vendor_id": "0x0000",
                    "product_id": "0x0000",
                    "interface": 0,
                    "report_id": 1,
                    "usage": "0xfffa0004",
                    "bitpos": 292,
                    "size": 12,
                    "relative": false,
                    "signed": false
                },
                {
                    "vendor_id": "0x0000",
                    "product_id": "0x0000",
                    "interface": 0,
                    "report_id": 1,
                    "usage": "0xfffa0005",
                    "bitpos": 304,
                    "size": 7,
                    "relative": false,
                    "signed": false
                },
                {
                    "vendor_id": "0x0000",
                    "product_id": "0x0000",
                    "interface": 0,
                    "report_id": 1,
                    "usage": "0xfffa0006",
                    "bitpos": 311,
                    "size": 1,
                    "relative": false,
                    "signed": false
                },
                {
                    "vendor_id": "0x0000",
                    "product_id": "0x0000",
                    "interface": 0,
                    "report_id": 1,
                    "usage": "0xfffa0007",
                    "bitpos": 312,
                    "size": 12,
                    "relative": false,
                    "signed": false
                },
                {
                    "vendor_id": "0x0000",
                    "product_id": "0x0000",
                    "interface": 0,
                    "report_id": 1,
                    "usage": "0xfffa0008",
                    "bitpos": 324,
                    "size": 12,
                    "relative": false,
                    "signed": false
                }
            ]
        }
    },
    {
        'description': 'analog stick cardinal directions to D-pad',
        'config': {
            "version": 17,
            "unmapped_passthrough_layers": [
                4,
                5,
                6,
                7
            ],
            "partial_scroll_timeout": 1000000,
            "tap_hold_threshold": 200000,
            "gpio_debounce_time_ms": 5,
            "interval_override": 0,
            "our_descriptor_number": 2,
            "ignore_auth_dev_inputs": false,
            "macro_entry_duration": 1,
            "gpio_output_mode": 0,
            "input_labels": 1,
            "mappings": [
                {
                    "source_usage": "0xfff5000b",
                    "target_usage": "0xfff90003",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff5000c",
                    "target_usage": "0xfff90004",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff5000d",
                    "target_usage": "0xfff90001",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff5000e",
                    "target_usage": "0xfff90002",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                }
            ],
            "macros": [
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                []
            ],
            "expressions": [
                "0x00010030 input_state_scaled eol 128000 sub eol dup mul eol eol 0x00010031 input_state_scaled eol 128000 sub eol dup mul eol eol add sqrt /* sqrt(x^2 + y^2) */ eol 64000 gt eol 7000 store /* stick outside deadzone? */ eol eol 0x00010030 input_state_scaled eol 128000 sub eol 0x00010031 input_state_scaled eol 128000 sub eol atan2 eol 8000 store /* stick angle */ eol eol /* overlapping (d-pad) */ eol /* up */ eol 8000 recall 112500 gt eol 8000 recall -112500 lt eol bitwise_or eol 7000 recall mul /* deadzone */ eol 11000 store eol eol /* down */ eol 8000 recall -67500 gt eol 8000 recall 67500 lt eol mul eol 7000 recall mul /* deadzone */ eol 12000 store eol eol /* left */ eol 8000 recall -157500 gt eol 8000 recall -22500 lt eol mul eol 7000 recall mul /* deadzone */ eol 13000 store eol eol /* right */ eol 8000 recall 22500 gt eol 8000 recall 157500 lt eol mul eol 7000 recall mul /* deadzone */ eol 14000 store",
                "",
                "",
                "",
                "",
                "",
                "",
                ""
            ],
            "quirks": []
        }
    },
    {
        'description': 'analog stick cardinal directions to buttons',
        'config': {
            "version": 17,
            "unmapped_passthrough_layers": [
                4,
                5,
                6,
                7
            ],
            "partial_scroll_timeout": 1000000,
            "tap_hold_threshold": 200000,
            "gpio_debounce_time_ms": 5,
            "interval_override": 0,
            "our_descriptor_number": 2,
            "ignore_auth_dev_inputs": false,
            "macro_entry_duration": 1,
            "gpio_output_mode": 0,
            "input_labels": 1,
            "mappings": [
                {
                    "source_usage": "0xfff50015",
                    "target_usage": "0x00090004",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff50016",
                    "target_usage": "0x00090002",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff50017",
                    "target_usage": "0x00090001",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff50018",
                    "target_usage": "0x00090003",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                }
            ],
            "macros": [
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                []
            ],
            "expressions": [
                "0x00010030 input_state_scaled eol 128000 sub eol dup mul eol eol 0x00010031 input_state_scaled eol 128000 sub eol dup mul eol eol add sqrt /* sqrt(x^2 + y^2) */ eol 64000 gt eol 7000 store /* stick outside deadzone? */ eol eol 0x00010030 input_state_scaled eol 128000 sub eol 0x00010031 input_state_scaled eol 128000 sub eol atan2 eol 8000 store /* stick angle */ eol eol /* non-overlapping (buttons) */ eol /* up */ eol 8000 recall 135000 gt eol 8000 recall -135000 lt eol bitwise_or eol 7000 recall mul /* deadzone */ eol 21000 store eol eol /* down */ eol 8000 recall -45000 gt eol 8000 recall 45000 lt eol mul eol 7000 recall mul /* deadzone */ eol 22000 store eol eol /* left */ eol 8000 recall -135000 gt eol 8000 recall -45000 lt eol mul eol 7000 recall mul /* deadzone */ eol 23000 store eol eol /* right */ eol 8000 recall 45000 gt eol 8000 recall 135000 lt eol mul eol 7000 recall mul /* deadzone */ eol 24000 store",
                "",
                "",
                "",
                "",
                "",
                "",
                ""
            ],
            "quirks": []
        }
    },
    {
        'description': 'analog stick to L2/R2',
        'config': {
            "version": 17,
            "unmapped_passthrough_layers": [
                4,
                5,
                6,
                7
            ],
            "partial_scroll_timeout": 1000000,
            "tap_hold_threshold": 200000,
            "gpio_debounce_time_ms": 5,
            "interval_override": 0,
            "our_descriptor_number": 4,
            "ignore_auth_dev_inputs": false,
            "macro_entry_duration": 1,
            "gpio_output_mode": 0,
            "input_labels": 1,
            "mappings": [
                {
                    "source_usage": "0xfff50001",
                    "target_usage": "0x000200c5",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff50002",
                    "target_usage": "0x00090014",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff50003",
                    "target_usage": "0x000200c4",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff50004",
                    "target_usage": "0x00090013",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                }
            ],
            "macros": [
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                []
            ],
            "expressions": [
                "0x00010031 input_state_scaled eol 127000 sub eol dup abs 8000 gt mul /* deadzone */ eol 2000 mul eol 0 255000 clamp eol dup eol 1000 store /* L2 axis */ eol 64000 gt eol 2000 store /* L2 button */ eol eol 0x00010031 input_state_scaled eol 128000 sub eol dup abs 8000 gt mul /* deadzone */ eol -2000 mul eol 0 255000 clamp eol dup eol 3000 store /* R2 axis */ eol 64000 gt eol 4000 store /* R2 button */",
                "",
                "",
                "",
                "",
                "",
                "",
                ""
            ],
            "quirks": []
        }
    },
    {
        'description': 'ROT13 keyboard',
        'config': {
            "version": 17,
            "unmapped_passthrough_layers": [
                0,
                1,
                2,
                3,
                4,
                5,
                6,
                7
            ],
            "partial_scroll_timeout": 1000000,
            "tap_hold_threshold": 200000,
            "gpio_debounce_time_ms": 5,
            "interval_override": 0,
            "our_descriptor_number": 0,
            "ignore_auth_dev_inputs": false,
            "macro_entry_duration": 1,
            "gpio_output_mode": 0,
            "input_labels": 0,
            "mappings": [
                {
                    "source_usage": "0x00070039",
                    "target_usage": "0xfff10001",
                    "layers": [
                        0
                    ],
                    "sticky": true,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff30001",
                    "target_usage": "0x00080002",
                    "layers": [
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0x00070004",
                    "target_usage": "0x00070011",
                    "layers": [
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0x00070005",
                    "target_usage": "0x00070012",
                    "layers": [
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0x00070006",
                    "target_usage": "0x00070013",
                    "layers": [
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0x00070007",
                    "target_usage": "0x00070014",
                    "layers": [
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0x00070008",
                    "target_usage": "0x00070015",
                    "layers": [
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0x00070009",
                    "target_usage": "0x00070016",
                    "layers": [
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0x0007000a",
                    "target_usage": "0x00070017",
                    "layers": [
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0x0007000b",
                    "target_usage": "0x00070018",
                    "layers": [
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0x0007000c",
                    "target_usage": "0x00070019",
                    "layers": [
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0x0007000d",
                    "target_usage": "0x0007001a",
                    "layers": [
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0x0007000e",
                    "target_usage": "0x0007001b",
                    "layers": [
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0x0007000f",
                    "target_usage": "0x0007001c",
                    "layers": [
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0x00070010",
                    "target_usage": "0x0007001d",
                    "layers": [
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0x00070011",
                    "target_usage": "0x00070004",
                    "layers": [
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0x00070012",
                    "target_usage": "0x00070005",
                    "layers": [
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0x00070013",
                    "target_usage": "0x00070006",
                    "layers": [
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0x00070014",
                    "target_usage": "0x00070007",
                    "layers": [
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0x00070015",
                    "target_usage": "0x00070008",
                    "layers": [
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0x00070016",
                    "target_usage": "0x00070009",
                    "layers": [
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0x00070017",
                    "target_usage": "0x0007000a",
                    "layers": [
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0x00070018",
                    "target_usage": "0x0007000b",
                    "layers": [
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0x00070019",
                    "target_usage": "0x0007000c",
                    "layers": [
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0x0007001a",
                    "target_usage": "0x0007000d",
                    "layers": [
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0x0007001b",
                    "target_usage": "0x0007000e",
                    "layers": [
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0x0007001c",
                    "target_usage": "0x0007000f",
                    "layers": [
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0x0007001d",
                    "target_usage": "0x00070010",
                    "layers": [
                        1
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 1000,
                    "source_port": 0,
                    "target_port": 0
                }
            ],
            "macros": [
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                []
            ],
            "expressions": [
                "1000",
                "",
                "",
                "",
                "",
                "",
                "",
                ""
            ],
            "quirks": []
        }
    },
    {
        'description': 'D-pad (hat switch) as analog stick',
        'config': {
            "version": 17,
            "unmapped_passthrough_layers": [
                0,
                1,
                2,
                3,
                4,
                5,
                6,
                7
            ],
            "partial_scroll_timeout": 1000000,
            "tap_hold_threshold": 200000,
            "gpio_debounce_time_ms": 5,
            "interval_override": 0,
            "our_descriptor_number": 2,
            "ignore_auth_dev_inputs": false,
            "macro_entry_duration": 1,
            "gpio_output_mode": 0,
            "input_labels": 1,
            "mappings": [
                {
                    "source_usage": "0xfff90001",
                    "target_usage": "0x00010032",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 0,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff90002",
                    "target_usage": "0x00010032",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 255000,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff90003",
                    "target_usage": "0x00010035",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 0,
                    "source_port": 0,
                    "target_port": 0
                },
                {
                    "source_usage": "0xfff90004",
                    "target_usage": "0x00010035",
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false,
                    "scaling": 255000,
                    "source_port": 0,
                    "target_port": 0
                }
            ],
            "macros": [
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                [],
                []
            ],
            "expressions": [
                "",
                "",
                "",
                "",
                "",
                "",
                "",
                ""
            ],
            "quirks": []
        }
    }
];

export default examples;
