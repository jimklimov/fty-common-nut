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

/**
 * \brief Generic entry point to get dump data from NUT drivers.
 *
 * \param[in] driver Name of the NUT driver (dummy-ups, snmp-ups, netxml-ups ..).
 * \param[in] extra Map of extra input arguments.
 * \param[in] loop_nb Number of loop the driver will try to collect data.
 * \param[out] out Map of key/value returned by driver with output.
 * \return 0 if success.
 */
KeyValues s_dumpDeviceData(
    const std::string& driver,
    const KeyValues& extra,
    unsigned loop_nb,
    unsigned loop_iter_time)
{
    MlmSubprocess::Argv args = {
        driver,
        "-d", std::to_string(loop_nb),
        "-u", "root",
        "-s", std::string("dumpdata-") + std::to_string(rand() % 100000 + 1)
    } ;
    std::string stdout, stderr;

    for (const auto& it : extra) {
        args.emplace_back("-x");
        args.emplace_back(it.first+"="+it.second);
    }

    (void)priv::runCommand(args, stdout, stderr, loop_nb*loop_iter_time);

    return parseDumpOutput(stdout);
}

KeyValues dumpDeviceSNMPv3(
    const std::string& port,
    const CredentialsSNMPv3& credentials,
    unsigned loop_nb,
    unsigned loop_iter_time)
{
    KeyValues extra = {
        { "port", port },
        { "snmp_version", "v3" },
        { "secName", credentials.secName }
    } ;
    
    if (!credentials.authPassword.empty()) {
        extra.emplace("authPassword", credentials.authPassword);
        if (!credentials.authProtocol.empty()) {
            extra.emplace("authProtocol", credentials.authProtocol);
        }

        if (!credentials.privPassword.empty()) {
            extra.emplace("privPassword", credentials.privPassword);
            if (!credentials.privProtocol.empty()) {
                extra.emplace("privProtocol", credentials.privProtocol);
            }

            extra.emplace("secLevel", "authPriv");
        }
        else {
            extra.emplace("secLevel", "authNoPriv");
        }
    }
    else {
        extra.emplace("secLevel", "noAuthNoPriv");
    }

    return s_dumpDeviceData("/lib/nut/snmp-ups", extra, loop_nb, loop_iter_time);
}

KeyValues dumpDeviceSNMPv1(
    const std::string& port,
    const CredentialsSNMPv1& credentials,
    unsigned loop_nb,
    unsigned loop_iter_time)
{
    KeyValues extra = {
        { "port", port },
        { "community", credentials.community }
    } ;

    return s_dumpDeviceData("/lib/nut/snmp-ups", extra, loop_nb, loop_iter_time);
}

KeyValues dumpDeviceNetXML(
    const std::string& port,
    unsigned loop_nb,
    unsigned loop_iter_time)
{
    KeyValues extra = {
        { "port", port }
    } ;

    return s_dumpDeviceData("/lib/nut/netxml-ups", extra, loop_nb, loop_iter_time);
}

KeyValues dumpDeviceDummy(
    const std::string& device,
    unsigned loop_nb,
    unsigned loop_iter_time)
{
    KeyValues extra = {
        { "port", device }
    } ;

    return s_dumpDeviceData("/lib/nut/dummy-ups", extra, loop_nb, loop_iter_time);
}

// Deprecated versions

int dumpDeviceSNMPv3(
    const std::string& port,
    const CredentialsSNMPv3& credentials,
    unsigned loop_nb,
    unsigned loop_iter_time,
    KeyValues& out)
{
    out = dumpDeviceSNMPv3(port, credentials, loop_nb, loop_iter_time);
    return out.empty();
}

int dumpDeviceSNMPv1(
    const std::string& port,
    const CredentialsSNMPv1& credentials,
    unsigned loop_nb,
    unsigned loop_iter_time,
    KeyValues& out)
{
    out = dumpDeviceSNMPv1(port, credentials, loop_nb, loop_iter_time);
    return out.empty();
}

int dumpDeviceNetXML(
    const std::string& port,
    unsigned loop_nb,
    unsigned loop_iter_time,
    KeyValues& out)
{
    out = dumpDeviceNetXML(port, loop_nb, loop_iter_time);
    return out.empty();
}

int dumpDeviceDummy(
    const std::string& device,
    unsigned loop_nb,
    unsigned loop_iter_time,
    KeyValues& out)
{
    out = dumpDeviceDummy(device, loop_nb, loop_iter_time);
    return out.empty();
}

}
