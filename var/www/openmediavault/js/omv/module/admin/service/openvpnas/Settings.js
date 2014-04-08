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
        },{
            xtype : "fieldset",
            title : _("Admin Password"),
            items : [{
                xtype       : "passwordfield",
                name        : "admin_pass",
                fieldLabel  : _("Admin Password"),
                allowBlank  : true,
                submitValue : false,
                plugins     : [{
                    ptype : "fieldinfo",
                    text  : _("Sets the admin user (openvpn) password.")
                }]
            },{
                xtype   : "button",
                name    : "setpasswd",
                text    : _("Set Password"),
                scope   : this,
                margin  : "5 0 5 0",
                handler : function() {
                    var me = this;
                    OMV.MessageBox.show({
                        title   : _("Confirmation"),
                        msg     : _("Are you sure you want to change the Admin user password?"),
                        buttons : Ext.Msg.YESNO,
                        fn      : function(answer) {
                            if (answer !== "yes")
                               return;

                            OMV.MessageBox.wait(null, _("Changing password..."));
                            OMV.Rpc.request({
                                scope   : me,
                                rpcData : {
                                    service : "OpenVPNAS",
                                    method  : "doChangePassword",
                                    params  : {
                                        admin_pass : me.getForm().findField("admin_pass").getValue()
                                    }
                                },
                                success : function(id, success, response) {
                                    me.doReload();
                                    OMV.MessageBox.hide();
                                }
                            });
                        },
                        scope : me,
                        icon  : Ext.Msg.QUESTION
                    });
                }
            }]
        },{
            xtype   : "fieldset",
            title   : _("Instructions"),
            layout  : "fit",
            items : [{
                border  : false,
                html    : 'When you enable the server you should be connected to OMV via http.  If you are connected via https your connection will be disrupted by the default server settings.' +
                          '  In any case, if you cannot connect to OMV via http after installing the plugin connect to the Admin Web Ui at <b>https://' + location.hostname + ':943/admin</b> until server configuration is complete.' +
                          '</p>' +
                          '<h3>Server Configuration</h3>' +
                          '<ol>' +
                          '<li>' +
                          'Before you can login to the Admin Web UI you need to create a password for the admin user.  This is done above.' +
                          '</li>' +
                          '<li>' +
                          'Now click on the Admin Web UI button on the Settings tab, or via <b>https://' + location.hostname + ':943/admin</b>, and login by entering <b>openvpn</b> and your <b>password</b>.' +
                          '</li>' +
                          '<li>' +
                          'Once logged in there are 3 settings we need to change.  In the left column under Configuration click on <b>Server Network Settings</b>.' +
                          '</li>' +
                          '<li>' +
                          'The first item you need to enter is <b>Hostname or IP Address:</b>.  You need to put the <b>WAN IP</b> of your OMV here.  You can obtain your WAN IP via this site <b>http://www.whatismyip.com</b>.  If you are using a <b>xxx.dyndns.org</b> type address to reach you home network you can substitute that address for dynamic ips.' +
                          '</li>' +
                          '<li>' +
                          'The second item needs to be adjusted.  Under <b>Protocol</b> put a bullet in <b>UDP</b> and make the <b>Port number: 1194</b>.' +
                          '</li>' +
                          '<li>' +
                          'For the last item you need to scroll to the bottom of the page and put check mark in <b>Disable SSLv2 Support</b>.' +
                          '</li>' +
                          '<li>' +
                          'Now click on <b>Save Settings</b>.  The OpenVPN Access Server should now be ready for use with OMV.' +
                          '</li>' +
                          '</ol>' +
                          '<h3>Router and Firewall Settings</h3>' +
                          '<ol>' +
                          '<li>' +
                          'You need to forward <b>Port 1194 UDP</b> and <b>Port 943 TCP</b> from your router to your OMV.' +
                          '</li>' +
                          '<li>' +
                          'If you are using the firewall on your OMV you need to create rules to open <b>Port 1194 UDP</b> and <b>Port 943 TCP</b>.' +
                          '</li>' +
                          '<li>' +
                          'Port 943 TCP must remain open even after client software installation.  The VPN connection uses both ports.' +
                          '</li>' +
                          '</ol>' +
                          '<h3>Client Software Installation and VPN Connection</h3>' +
                          '<ol>' +
                          '<li>' +
                          'Connect at <b>https://wanipofyouromv:943</b> and sign in with an OMV UserID and Password.  Then click the <b>Go</b> button.  Any OMV user will be allowed unless you have put a check mark on <b>Deny access to all users not listed above</b> in the Users Permissions section of the Admin Web UI.' +
                          '</li>' +
                          '<li>' +
                          'Once connected you will be prompted to download and install the Connect Client software.  Download it and install.  After installation right click on the icon in the system tray and click on connect.  The client should establish a connection.  Autologin is not recommended.' +
                          '</li>' +
                          '</ol>'
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
