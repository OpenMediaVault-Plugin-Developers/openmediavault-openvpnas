<?php
/**
 * Copyright (C) 2015-2016 OpenMediaVault Plugin Developers.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
use OMV\System\LogFileSpec;

LogFileSpec::registerSpecification(
    'openvpnas', array
    [
        'filename' => 'openvpnas.log',
        'filepath' => '/var/log/openvpnas.log',
        'regex' => '/^(\d+-\d+-\d+\s+\d+:\d+:\d+[-\+]\d+)\s+\[.*\]\s+(.*)$/',
        'columns' => [
            'date' => array [
                'index' => 1,
                'func' => create_function ($v) {
                    return strpdate($v, 'Y-m-d H:i:sO');
                },
            ],
            'event' => 2,
        ],
    ]
);
