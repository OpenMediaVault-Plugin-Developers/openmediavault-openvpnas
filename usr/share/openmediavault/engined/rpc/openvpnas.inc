<?php
/**
 * @license   http://www.gnu.org/licenses/gpl.html GPL Version 3
 * @author    OpenMediaVault Plugin Developers <plugins@omv-extras.org>
 * @copyright Copyright (c) 2013-2016 OpenMediaVault Plugin Developers
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
require_once("openmediavault/object.inc");
require_once("openmediavault/config.inc");
require_once("openmediavault/error.inc");
require_once("openmediavault/util.inc");
require_once("openmediavault/rpc.inc");
require_once("openmediavault/notify.inc");

class OMVRpcServiceOpenVPNAS extends OMVRpcServiceAbstract {
    /**
     * Get the RPC service name.
     */
    public function getName() {
        return "OpenVPNAS";
    }

    /**
     * Initialize the RPC service.
     */
    public function initialize() {
        $this->registerMethod("getSettings");
        $this->registerMethod("setSettings");
        $this->registerMethod("doChangePassword");
    }

    /**
     * Get the settings configuration object.
     * @param params The method parameters.
     * @param context The context of the caller.
     * @return The requested configuration object.
     * @throw E_CONFIG_GET_OBJECT_FAILED
     */
    public function getSettings($params, $context) {
        global $xmlConfig;
        // Validate the RPC caller context.
        $this->validateMethodContext($context, [ "role" => OMV_ROLE_ADMINISTRATOR ]);

        // Get the configuration object.
        $xpath = "//services/openvpnas";
        $object = $xmlConfig->get($xpath);
        if(is_null($object))
            throw new OMVException(OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED, $xpath);

        // Modify the result object.
        $object['enable']  = boolval($object['enable']);
        $object['showtab'] = boolval($object['showtab']);

        return $object;
    }

    /**
     * Set the settings configuration object.
     * @param params The method parameters.
     * @param context The context of the caller.
     * @return The stored configuration object.
     * @throw E_CONFIG_GET_OBJECT_FAILED
     * @throw E_CONFIG_SET_OBJECT_FAILED
     */
    public function setSettings($params, $context) {
        global $xmlConfig;
        // Validate the RPC caller context.
        $this->validateMethodContext($context, [ "role" => OMV_ROLE_ADMINISTRATOR ]);

        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, '{
            "type"       : "object",
            "properties" : {
                "enable"    : { "type" : "boolean" },
                "showtab"   : { "type" : "boolean" }
            }
        }');

        // Prepare configuration data
        $object = array(
            "enable"    => array_boolval($params, 'enable'),
            "showtab"   => array_boolval($params, 'showtab')
        );

        // Set configuration object
        if (false === $xmlConfig->replace("//services/openvpnas", $object))
            throw new OMVException(OMVErrorMsg::E_CONFIG_SET_OBJECT_FAILED, $this->getXpath());

        // Notify configuration changes
        $dispatcher = &OMVNotifyDispatcher::getInstance();
        $dispatcher->notify(OMV_NOTIFY_MODIFY, "org.openmediavault.services.openvpnas", $object);

        return $object;
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
            throw new OMVException(OMVErrorMsg::E_EXEC_FAILED, $cmd, implode("\n", $output));
    }    
}

// Register the RPC service.
$rpcServiceMgr = &OMVRpcServiceMgr::getInstance();
$rpcServiceMgr->registerService(new OMVRpcServiceOpenVPNAS());
