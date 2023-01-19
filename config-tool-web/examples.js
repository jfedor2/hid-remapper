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
];

export default examples;
