<?php
/**
 * @license   http://www.gnu.org/licenses/gpl.html GPL Version 3
 * @author    OpenMediaVault Plugin Developers <plugins@omv-extras.org>
 * @copyright Copyright (C) 2014-2016 OpenMediaVault Plugin Developers
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

require_once "openmediavault/module.inc";
require_once "openmediavault/config.inc";
require_once "openmediavault/error.inc";
require_once "openmediavault/util.inc";
require_once "openmediavault/systemctl.inc";
require_once "openmediavault/log.inc";

class OMVModuleOpenVPNAS extends OMVModuleServiceAbstract
    implements OMVINotifyListener, OMVIModuleServiceStatus
{
    /* Helpers */

    private function getEventMessagePath()
    {
        return "org.openmediavault.services.openvpnas";
    }

    private function getXPath()
    {
        return "/config/services/openvpnas";
    }

    /* /Helpers */

    /**
     * Get the module name.
     */
    public function getName()
    {
        return "openvpnas";
    }

    /**
     * Get the status of the service
     */
    public function getStatus()
    {
        global $xmlConfig;

        $object = $xmlConfig->get($this->getXPath());
        if (is_null($object))
            throw new OMVException(OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED, $this->getXPath());

        $systemCtl = new OMVSystemCtl( $this->getName() );
        return [
            "name"    => $this->getName(),
            "title"   => gettext("OpenVPN AS"),
            "enabled" => boolval($object["enable"]),
            "running" => $systemCtl->isActive(),
        ];
    }

    /**
     * Create and update the configuration
     */
    public function applyConfig()
    {
        $cmd = sprintf("export LANG=C ; omv-mkconf %s 2>&1", $this->getName());

        if (0 !== $this->exec($cmd, $output))
            throw new OMVException(OMVErrorMsg::E_EXEC_FAILED,
                $cmd, implode(PHP_EOL, $output));
    }

    /**
     * Start the service
     */
    public function startService()
    {
        global $xmlConfig;

        $object = $xmlConfig->get($this->getXPath());

        if (is_null($object))
            throw new OMVException(OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED,
                $this->getXPath());

        if (!$object["enable"])
            return;

        $systemCtl = new OMVSystemCtl($this->getName(), true);
        $systemCtl->enable(true);
    }

    /**
     * Stop the service
     */
    public function stopService()
    {
        $systemCtl = new OMVSystemCtl($this->getName());
        $systemCtl->disable(true);
    }

    /**
     * Bind listeners.
     */
    public function bindListeners(OMVNotifyDispatcher $dispatcher) {
        $dispatcher->addListener(
            OMV_NOTIFY_MODIFY,
            $this->getEventMessagePath(),
            array($this, "setDirty"));
    }
}

// Register module
$moduleMgr = &OMVModuleMgr::getInstance();
$moduleMgr->registerModule(new OMVModuleOpenVPNAS());

// Register new logfile type
OMVLogFile::registerType("openvpnas", array(
    "filename" => "openvpnas.log",
    "filepath" => "/var/log/openvpnas.log",
    "regex"    => "/^(\d+-\d+-\d+\s+\d+:\d+:\d+[-\+]\d+)\s+\[.*\]\s+(.*)$/",
    "columns"  => array(
        "date"  => array(
            "index" => 1,
            "func"  => create_function("\$v", "return strpdate(\$v, \"Y-m-d H:i:sO\");")
        ),
        "event" => 2
    )
));
