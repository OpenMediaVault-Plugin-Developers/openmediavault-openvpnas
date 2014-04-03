/**
 * @license   http://www.gnu.org/licenses/gpl.html GPL Version 3
 * @author    Volker Theile <volker.theile@openmediavault.org>
 * @author    OpenMediaVault Plugin Developers <plugins@omv-extras.org>
 * @copyright Copyright (c) 2009-2013 Volker Theile
 * @copyright Copyright (c) 2013-2014 OpenMediaVault Plugin Developers
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
// require("js/omv/WorkspaceManager.js")
// require("js/omv/workspace/form/Panel.js")
// require("js/omv/workspace/window/Form.js")
// require("js/omv/data/Store.js")
// require("js/omv/data/Model.js")
// require("js/omv/data/proxy/Rpc.js")
// require("js/omv/workspace/window/plugin/ConfigObject.js")
// require("js/omv/form/field/SharedFolderComboBox.js")

/**
 * @class OMV.module.admin.service.openvpnas.Settings
 * @derived OMV.workspace.form.Panel
 */
Ext.define("OMV.module.admin.service.openvpnas.Settings", {
    extend : "OMV.workspace.form.Panel",

    rpcService   : "OpenVPNAS",
    rpcGetMethod : "getSettings",
    rpcSetMethod : "setSettings",
    plugins      : [{
        ptype        : "linkedfields",
        correlations : [{
            name       : [
                "openmanage"
            ],
            conditions : [
                { name  : "enable", value : false }
            ],
            properties : "disabled"
        }]
    }],

    initComponent : function () {
        var me = this;

        me.on('load', function () {
            var checked = me.findField('enable').checked;
            var showtab = me.findField('showtab').checked;
            var parent = me.up('tabpanel');

            if (!parent)
                return;

            var webClientPanel = parent.down('panel[title=' + _("Admin Web UI") + ']');

            if (webClientPanel) {
                checked ? webClientPanel.enable() : webClientPanel.disable();
                showtab ? webClientPanel.tab.show() : webClientPanel.tab.hide();
            }
        });
        me.callParent(arguments);
    },

    getFormItems: function () {
        return [{
            xtype : "fieldset",
            title : _("General settings"),
            items : [{
                xtype      : "checkbox",
                name       : "enable",
                fieldLabel : _("Enable"),
                checked    : false
            },{
                xtype      : "checkbox",
                name       : "showtab",
                fieldLabel : _("Enable"),
                boxLabel   : _("Show tab with Admin Web UI."),
                checked    : false
        },{
            xtype   : "button",
            name    : "openmanage",
            text    : _("Admin Web UI"),
            scope   : this,
            handler : function() {
                var link = 'https://' + location.hostname + ':943/admin';
                window.open(link, '_blank');
            },
            margin  : "0 0 5 0"
        }]
      }];
   }
});

OMV.WorkspaceManager.registerPanel({
    id        : "settings",
    path      : "/service/openvpnas",
    text      : _("Settings"),
    position  : 10,
    className : "OMV.module.admin.service.openvpnas.Settings"
});