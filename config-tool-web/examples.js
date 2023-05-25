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
                "0x00010030 input_state -128000 add dup abs 10000 gt mul 25 mul auto_repeat mul",
                "0x00010031 input_state -128000 add dup abs 10000 gt mul 25 mul auto_repeat mul",
                "0x00010033 input_state 1 mul 0x00010034 input_state -1 mul add 250 mul auto_repeat mul",
                "0x00010035 input_state -128000 add dup abs 10000 gt mul -1 mul 250 mul auto_repeat mul",
                "0x00010039 input_state 7000 gt not 0x00010039 input_state 45000 mul sin 1000 mul mul auto_repeat mul",
                "0x00010039 input_state 7000 gt not 0x00010039 input_state 45000 mul cos -1000 mul mul auto_repeat mul",
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
                "time 2000000 mod 500000 gt not time 500000 mod 720 mul cos mul auto_repeat mul",
                "time 2000000 mod 500000 gt not time 500000 mod 720 mul sin -1000 mul mul auto_repeat mul",
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
];

export default examples;
