/*  =========================================================================
    fty_common_nut_credentials - class description

    Copyright (C) 2014 - 2018 Eaton

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
    =========================================================================
*/

/*
@header
    fty_common_nut_credentials -
@discuss
@end
*/

#include "fty_common_nut_classes.h"

namespace nutcommon {

const char *FTY_DEFAULT_CFG_FILE = "/etc/default/fty.cfg";

std::vector<CredentialsSNMPv3> getCredentialsSNMPv3()
{
    std::vector<CredentialsSNMPv3> creds;

    zconfig_t *config = zconfig_load(FTY_DEFAULT_CFG_FILE);
    if (config) {
        zconfig_t *item = zconfig_locate(config, "snmpv3");
        if (item) {
            zconfig_t *child = zconfig_child(item);
            while (child) {
                const char *secName = zconfig_get(child, "secName", nullptr);
                const char *authPassword = zconfig_get(child, "authPassword", "");
                const char *authProtocol = zconfig_get(child, "authProtocol", "");
                const char *privPassword = zconfig_get(child, "privPassword", "");
                const char *privProtocol = zconfig_get(child, "privProtocol", "");

                if (secName) {
                    creds.emplace_back(secName, authPassword, authProtocol, privPassword, privProtocol);
                }

                child = zconfig_next(child);
            }
        }
    }
    else {
        log_warning("Config file '%s' could not be read.", FTY_DEFAULT_CFG_FILE);
    }
    zconfig_destroy(&config);

    return creds;
}

std::vector<CredentialsSNMPv1> getCredentialsSNMPv1()
{
    std::vector<CredentialsSNMPv1> creds;

    zconfig_t *config = zconfig_load(FTY_DEFAULT_CFG_FILE);
    if (config) {
        zconfig_t *item = zconfig_locate(config, "snmp/community");
        if (item) {
            zconfig_t *child = zconfig_child(item);
            while (child) {
                const char *community = zconfig_value (child);
                creds.emplace_back(community);
                child = zconfig_next(child);
            }
        }
    }
    else {
        log_warning("Config file '%s' could not be read.", FTY_DEFAULT_CFG_FILE);
    }
    zconfig_destroy(&config);

    // Fallback.
    if (creds.empty()) {
        creds.emplace_back("public");
    }

    return creds;
}

}