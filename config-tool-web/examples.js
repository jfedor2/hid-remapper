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
            "version": 9,
            "unmapped_passthrough_layers": [],
            "partial_scroll_timeout": 1000000,
            "tap_hold_threshold": 200000,
            "gpio_debounce_time_ms": 5,
            "interval_override": 0,
            "our_descriptor_number": 2,
            "ignore_auth_dev_inputs": false,
            "mappings": [
                {
                    "target_usage": "0x00010039",
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
                    "target_usage": "0x00010032",
                    "source_usage": "0xfff30007",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false
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
                    "hold": false
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
                    "hold": false
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
                    "hold": false
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
                    "hold": false
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
                    "hold": false
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
                    "hold": false
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
                    "hold": false
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
                    "hold": false
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
                    "hold": false
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
                    "hold": false
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
                    "hold": false
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
                    "hold": false
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
                    "hold": false
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
                    "hold": false
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
                    "hold": false
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
                    "hold": false
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
                    "hold": false
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
                    "hold": false
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
                    "hold": false
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
                    "hold": false
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
                    "hold": false
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
                    "hold": false
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
                    "hold": false
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
                    "hold": false
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
                    "hold": false
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
                "8000",
                "",
                "0x00070004 input_state_binary 1 mul 0x00070007 input_state_binary 2 mul bitwise_or 0x0007001a input_state_binary 4 mul bitwise_or 0x00070016 input_state_binary 8 mul bitwise_or 1000 store -1000 1000 recall 1 eq 7000 mul add 1000 recall 2 eq 3000 mul add 1000 recall 4 eq 1000 mul add 1000 recall 8 eq 5000 mul add 1000 recall 5 eq 8000 mul add 1000 recall 6 eq 2000 mul add 1000 recall 10 eq 4000 mul add 1000 recall 9 eq 6000 mul add 1000 recall 7 eq 1000 mul add 1000 recall 11 eq 5000 mul add 1000 recall 13 eq 7000 mul add 1000 recall 14 eq 3000 mul add 45000 mul dup dup dup dup 135000 gt 2000 mul -1000 add 4000 store 315000 add 360000 mod 135000 gt 2000 mul -1000 add 5000 store -45000 eq not dup 7000 store 0 0x00070014 input_state_binary 15000 mul 4000 recall mul add 0x00070008 input_state_binary 15000 mul 5000 recall mul add 0x0007001d input_state_binary -15000 mul 5000 recall mul add 0x00070006 input_state_binary -15000 mul 4000 recall mul add mul add 3000 store 0x000700e1 input_state_binary -500 mul 1000 add 6000 store",
                "3000 recall sin 127000 mul 6000 recall mul 7000 recall mul 128000 add",
                "3000 recall cos -127000 mul 6000 recall mul 7000 recall mul 128000 add",
                "0x00070018 input_state_binary 0x00070059 input_state_binary bitwise_or 1 mul 0x00070012 input_state_binary 0x0007005b input_state_binary bitwise_or 2 mul bitwise_or 0x00070013 input_state_binary 0x00070058 input_state_binary bitwise_or 4 mul bitwise_or 0x0007000c input_state_binary 0x0007005a input_state_binary bitwise_or 8 mul bitwise_or 10000 store -1000 10000 recall 1 eq 7000 mul add 10000 recall 2 eq 3000 mul add 10000 recall 4 eq 1000 mul add 10000 recall 8 eq 5000 mul add 10000 recall 5 eq 8000 mul add 10000 recall 6 eq 2000 mul add 10000 recall 10 eq 4000 mul add 10000 recall 9 eq 6000 mul add 10000 recall 7 eq 1000 mul add 10000 recall 11 eq 5000 mul add 10000 recall 13 eq 7000 mul add 10000 recall 14 eq 3000 mul add 45000 mul dup -45000 eq not 8000 store 9000 store",
                "9000 recall sin 127000 mul 8000 recall mul 128000 add",
                "9000 recall cos -127000 mul 8000 recall mul 128000 add"
            ]
        }
    },
    {
        'description': 'Stadia controller adapter for PS4',
        'config': {
            "version": 11,
            "unmapped_passthrough_layers": [],
            "partial_scroll_timeout": 1000000,
            "tap_hold_threshold": 200000,
            "gpio_debounce_time_ms": 5,
            "interval_override": 0,
            "our_descriptor_number": 3,
            "ignore_auth_dev_inputs": true,
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
                    "target_usage": "0x00010039",
                    "source_usage": "0x00010039",
                    "source_name": "D-pad",
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
            "gpio_output_mode": 0
        }
    },
    {
        'description': 'keyboard adapter for PS4: arrows=D-pad, WASD=left stick, numpad=right stick',
        'config': {
            "version": 11,
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
            "mappings": [
                {
                    "target_usage": "0x00010039",
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
                    "source_usage": "0xfff5000a",
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
                    "source_usage": "0xfff5000b",
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
                    "source_usage": "0xfff5000c",
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
                    "source_usage": "0xfff5000d",
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
                "0x00070050 input_state_binary 0x0007004f input_state_binary 0x00070052 input_state_binary 0x00070051 input_state_binary dpad",
                "0x00070004 input_state_binary 0x00070007 input_state_binary 0x0007001a input_state_binary 0x00070016 input_state_binary dpad 45000 mul dup 360000 eq not 3000 store 4000 store",
                "4000 recall sin 127000 mul 3000 recall mul 128000 add",
                "4000 recall cos -127000 mul 3000 recall mul 128000 add",
                "0x0007005c input_state_binary 0x0007005e input_state_binary 0x00070060 input_state_binary 0x0007005d input_state_binary dpad 45000 mul dup 360000 eq not 6000 store 7000 store",
                "7000 recall sin 127000 mul 6000 recall mul 128000 add",
                "7000 recall cos -127000 mul 6000 recall mul 128000 add",
                "0x0007000f input_state_binary dup 10000 store 255000 mul 11000 store 0x00070033 input_state_binary dup 12000 store 255000 mul 13000 store"
            ],
            "macro_entry_duration": 1,
            "gpio_output_mode": 0
        }
    },
    {
        'description': 'mouse to analog stick',
        'config': {
            "version": 9,
            "unmapped_passthrough_layers": [],
            "partial_scroll_timeout": 1000000,
            "tap_hold_threshold": 200000,
            "gpio_debounce_time_ms": 5,
            "interval_override": 0,
            "our_descriptor_number": 2,
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
                    "target_usage": "0x00010032",
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
                    "target_usage": "0x00010035",
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
                    "target_usage": "0x00010039",
                    "source_usage": "0xfff30004",
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
                "0x00010030 input_state 1000 recall add -127000 127000 clamp 980 mul dup 1000 store 128000 add",
                "0x00010031 input_state 2000 recall add -127000 127000 clamp 980 mul dup 2000 store 128000 add",
                "128000",
                "8000",
                "",
                "",
                "",
                ""
            ]
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
            "version": 9,
            "unmapped_passthrough_layers": [],
            "partial_scroll_timeout": 1000000,
            "tap_hold_threshold": 200000,
            "gpio_debounce_time_ms": 5,
            "interval_override": 0,
            "our_descriptor_number": 2,
            "ignore_auth_dev_inputs": false,
            "mappings": [
                {
                    "target_usage": "0x00010039",
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
                    "target_usage": "0x00010030",
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
                    "target_usage": "0x00010031",
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
                    "target_usage": "0x00010032",
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
                    "target_usage": "0x00010035",
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
                    "target_usage": "0x00090002",
                    "source_usage": "0x00090001",
                    "source_name": "A",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false
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
                    "hold": false
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
                    "hold": false
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
                    "hold": false
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
                    "hold": false
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
                    "hold": false
                },
                {
                    "target_usage": "0x00090007",
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
                    "target_usage": "0x00090008",
                    "source_usage": "0xfff30007",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false
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
                    "hold": false
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
                    "hold": false
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
                    "hold": false
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
                    "hold": false
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
                "0x00010039 input_state -1000 add 9000 mod",
                "0x00010030 input_state 125 mul 125 mul 250 mul",
                "0x00010031 input_state 125 mul 125 mul 250 mul",
                "0x00010032 input_state 125 mul 125 mul 250 mul",
                "0x00010035 input_state 125 mul 125 mul 250 mul",
                "0x000200c5 input_state 1023000 eq",
                "0x000200c4 input_state 1023000 eq",
                ""
            ]
        }
    },
    {
        'description': 'Xbox controller (USB) adapter for Switch',
        'config': {
            "version": 9,
            "unmapped_passthrough_layers": [],
            "partial_scroll_timeout": 1000000,
            "tap_hold_threshold": 200000,
            "gpio_debounce_time_ms": 5,
            "interval_override": 0,
            "our_descriptor_number": 2,
            "ignore_auth_dev_inputs": false,
            "mappings": [
                {
                    "target_usage": "0x00010039",
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
                    "target_usage": "0x00010030",
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
                    "target_usage": "0x00010031",
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
                    "target_usage": "0x00010032",
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
                    "target_usage": "0x00010035",
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
                    "target_usage": "0x00090002",
                    "source_usage": "0x00090005",
                    "source_name": "A",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false
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
                    "hold": false
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
                    "hold": false
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
                    "hold": false
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
                    "hold": false
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
                    "hold": false
                },
                {
                    "target_usage": "0x00090007",
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
                    "target_usage": "0x00090008",
                    "source_usage": "0xfff30007",
                    "scaling": 1000,
                    "layers": [
                        0
                    ],
                    "sticky": false,
                    "tap": false,
                    "hold": false
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
                    "hold": false
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
                    "hold": false
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
                    "hold": false
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
                    "hold": false
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
                "0x0009000b input_state_binary 1 mul 0x0009000c input_state_binary 2 mul bitwise_or 0x00090009 input_state_binary 4 mul bitwise_or 0x0009000a input_state_binary 8 mul bitwise_or 1000 store -1000 1000 recall 1 eq 7000 mul add 1000 recall 2 eq 3000 mul add 1000 recall 4 eq 1000 mul add 1000 recall 8 eq 5000 mul add 1000 recall 5 eq 8000 mul add 1000 recall 6 eq 2000 mul add 1000 recall 10 eq 4000 mul add 1000 recall 9 eq 6000 mul add 1000 recall 7 eq 1000 mul add 1000 recall 11 eq 5000 mul add 1000 recall 13 eq 7000 mul add 1000 recall 14 eq 3000 mul add dup -1000 eq 9000 mul add",
                "0x00010030 input_state 32768000 add 125 mul 125 mul 250 mul",
                "0x00010031 input_state -32767000 add 125 mul 125 mul -250 mul",
                "0x00010032 input_state 32768000 add 125 mul 125 mul 250 mul",
                "0x00010035 input_state -32767000 add 125 mul 125 mul -250 mul",
                "0x000200c5 input_state 1023000 eq",
                "0x000200c4 input_state 1023000 eq",
                ""
            ]
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
            "our_descriptor_number": 4,
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
                "0xfff80000 input_state eol -2048000 add eol 100 mul eol 128000 add eol 0 255000 clamp",
                "0xfff80001 input_state eol -2048000 add eol -100 mul eol 128000 add eol 0 255000 clamp",
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
    ,
    {
        'description': 'custom board v8: analog stick as mouse',
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
                "0xfff80000 input_state eol -2048000 add eol dup eol abs 100000 gt mul eol 2 mul",
                "0xfff80001 input_state eol -2048000 add eol dup eol abs 100000 gt mul eol -2 mul",
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
