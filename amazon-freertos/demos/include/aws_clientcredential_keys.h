
/*
 * FreeRTOS V201906.00 Major
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

#ifndef AWS_CLIENT_CREDENTIAL_KEYS_H
#define AWS_CLIENT_CREDENTIAL_KEYS_H

/*
 * @brief PEM-encoded client certificate.
 *
 * @todo If you are running one of the FreeRTOS demo projects, set this
 * to the certificate that will be used for TLS client authentication.
 *
 * @note Must include the PEM header and footer:
 * "-----BEGIN CERTIFICATE-----
"\
 * "...base64 data...
" * "-----END CERTIFICATE-----
"
 */
#define keyCLIENT_CERTIFICATE_PEM   \
"-----BEGIN CERTIFICATE-----\n"\
"MIIBqjCCAU+gAwIBAgIQUJM5DdreiK2hkfF0ltDeBzAKBggqhkjOPQQDAjA0MRQw\n"\
"EgYDVQQKDAtFeGFtcGxlIEluYzEcMBoGA1UEAwwTRXhhbXBsZSBTaWduZXIgRkZG\n"\
"RjAgFw0yMDExMDIxMjAwMDBaGA8zMDAwMTIzMTIzNTk1OVowMzEUMBIGA1UECgwL\n"\
"RXhhbXBsZSBJbmMxGzAZBgNVBAMMEjAxMjMwMzczMDQ2OTA2MDFFRTBZMBMGByqG\n"\
"SM49AgEGCCqGSM49AwEHA0IABASCZEqyPxZtHZ+X6Rv2ZBQokdch4zkbM1PiPQuJ\n"\
"rMq9OZAWeSBXr3UGyJ/XeQq/yemDa8xzQ9zsoeKZsLvZS7OjQjBAMB0GA1UdDgQW\n"\
"BBQWb8J/dX4xngihho1qjgx1yKDs8TAfBgNVHSMEGDAWgBRu+WfGg0NkLrvMFDwm\n"\
"tmCyXPK9XjAKBggqhkjOPQQDAgNJADBGAiEAsbQKiFlpoAMRzPNPVU7out/WQZ6B\n"\
"jtV0JKBSdD3VTGkCIQCo30SnPWc34TTA4q5FG0NLgAqpjtCY05OR4b1QyTmlLw==\n"\
"-----END CERTIFICATE-----\n"\

/*
 * @brief PEM-encoded issuer certificate for AWS IoT Just In Time Registration (JITR).
 *
 * @todo If you are using AWS IoT Just in Time Registration (JITR), set this to
 * the issuer (Certificate Authority) certificate of the client certificate above.
 *
 * @note This setting is required by JITR because the issuer is used by the AWS
 * IoT gateway for routing the device's initial request. (The device client
 * certificate must always be sent as well.) For more information about JITR, see:
 *  https://docs.aws.amazon.com/iot/latest/developerguide/jit-provisioning.html,
 *  https://aws.amazon.com/blogs/iot/just-in-time-registration-of-device-certificates-on-aws-iot/.
 *
 * If you're not using JITR, set below to NULL.
 *
 * Must include the PEM header and footer:
 * "-----BEGIN CERTIFICATE-----
"\
 * "...base64 data...
" * "-----END CERTIFICATE-----
"
 */
#define keyJITR_DEVICE_CERTIFICATE_AUTHORITY_PEM  \
"-----BEGIN CERTIFICATE-----\n"\
"MIIByTCCAW6gAwIBAgIQf3WBFWnJJvDJkL63SoRSwTAKBggqhkjOPQQDAjAwMRQw\n"\
"EgYDVQQKDAtFeGFtcGxlIEluYzEYMBYGA1UEAwwPRXhhbXBsZSBSb290IENBMB4X\n"\
"DTIwMTEwMjA3MzMwNloXDTMwMTEwMjA3MzMwNlowNDEUMBIGA1UECgwLRXhhbXBs\n"\
"ZSBJbmMxHDAaBgNVBAMME0V4YW1wbGUgU2lnbmVyIEZGRkYwWTATBgcqhkjOPQIB\n"\
"BggqhkjOPQMBBwNCAAT62sWXd/6yKFtDYl3Ks3sKKczc+tTXHMDoiZnhtfWpNhK3\n"\
"pBUArLg/DzVjCr82aEz2CEbIXovl1nMTxNUmYp45o2YwZDASBgNVHRMBAf8ECDAG\n"\
"AQH/AgEAMA4GA1UdDwEB/wQEAwIBhjAdBgNVHQ4EFgQUbvlnxoNDZC67zBQ8JrZg\n"\
"slzyvV4wHwYDVR0jBBgwFoAUkDh+V3Hoq6KWBk9n2PvuhIxqfpIwCgYIKoZIzj0E\n"\
"AwIDSQAwRgIhAJxxDEPdH7eNDyUC1ZHKV9yYb11nPl9aW/1WbmQVh/wiAiEA/Oxw\n"\
"9ZafC5suip83f7yw7dgL9inCab+uTpJqT5uQ03s=\n"\
"-----END CERTIFICATE-----\n"\

/*
 * @brief PEM-encoded client private key.
 *
 * @todo If you are running one of the FreeRTOS demo projects, set this
 * to the private key that will be used for TLS client authentication.
 *
 * @note Must include the PEM header and footer:
 * "-----BEGIN RSA PRIVATE KEY-----
"\
 * "...base64 data...
" * "-----END RSA PRIVATE KEY-----
"
 */
#define keyCLIENT_PRIVATE_KEY_PEM                   ""

#endif /* AWS_CLIENT_CREDENTIAL_KEYS_H */

