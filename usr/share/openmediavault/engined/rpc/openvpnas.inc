<?php
/**
 * Copyright (C) 2014-2016 OpenMediaVault Plugin Developers.
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
namespace OMV\Engined\Rpc;

use OMV\Config\Database;
use OMV\Rpc\ServiceAbstract;

class OpenVPNAS extends ServiceAbstract
{
    /** @var Database */
    private $database;
    /** @var string */
    private $dataModelPath = 'conf.service.openvpnas';
    public function __construct(Database $database = null)
    {
        $this->database = $database ?: Database::getInstance();
    }
    /**
     * Get the name of the RPC service.
     *
     * @return string
     */
    public function getName()
    {
        return 'OpenVPNAS';
    }
    /**
     * Initialize the RPC service.
     *
     * @return void
     */
    public function initialize()
    {
        $this->registerMethod('getSettings');
        $this->registerMethod('setSettings');
        $this->registerMethod("doChangePassword");
    }
    /**
     * Get the settings of the service.
     *
     * @param array $params  The method paramaters.
     * @param array $context The context of the caller.
     *
     * @return array
     */
    public function getSettings($params, $context)
    {
        $this->validateMethodContext($context, ['role' => OMV_ROLE_ADMINISTRATOR]);
        $object = $this->database->get($this->dataModelPath);
        return $object->getAssoc();
    }
    /**
     * Set the settings of the service.
     *
     * @param array $params  The method paramaters.
     * @param array $context The context of the caller.
     *
     * @return array
     */
    public function setSettings($params, $context)
    {
        $this->validateMethodContext($context, ['role' => OMV_ROLE_ADMINISTRATOR]);
        $this->validateMethodParams($params, 'rpc.openvpnas.setsettings');
        $object = $this->database->get($this->dataModelPath);
        $object->setAssoc($params);
        $this->database->set($object);
        return $object->getAssoc();
    }
    
    public function doChangePassword($params, $context) {
        global $xmlConfig;
        // Validate the RPC caller context.
        $this->validateMethodContext($context, [ "role" => OMV_ROLE_ADMINISTRATOR ]);

        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, '{
            "type"       : "object",
            "properties" : {
                "admin_pass" : { "type" : "string" }
            }
        }');

        // create password file
        $file = "/tmp/.openmediavault-openvpnas-pwd";
        file_put_contents($file, $params['admin_pass']);

        // change password
        $cmd = "/usr/sbin/change_openvpnas_pwd 2>&1";
        if(0 !== $this->exec($cmd, $output))
            throw new OMVException(OMVErrorMsg::E_EXEC_FAILED, $cmd, implode(PHP_EOL, $output));
    } 
}
