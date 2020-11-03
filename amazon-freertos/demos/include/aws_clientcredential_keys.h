
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
"MIIBqTCCAU+gAwIBAgIQcs4n999E0jTwRemTB4bhKjAKBggqhkjOPQQDAjA0MRQw\n"\
"EgYDVQQKDAtFeGFtcGxlIEluYzEcMBoGA1UEAwwTRXhhbXBsZSBTaWduZXIgRkZG\n"\
"RjAgFw0yMDEwMzEwOTAwMDBaGA8zMDAwMTIzMTIzNTk1OVowMzEUMBIGA1UECgwL\n"\
"RXhhbXBsZSBJbmMxGzAZBgNVBAMMEjAxMjM3QjAyNUNBOTVCOEYwMTBZMBMGByqG\n"\
"SM49AgEGCCqGSM49AwEHA0IABLmSY0OkVfY+d/F3fTkHg+10GC5bseswbcv3DSR9\n"\
"xOvDqr87jO0s9MVZqEjQbNakKHvU96zIpd4XjZSp3W3QD1ejQjBAMB0GA1UdDgQW\n"\
"BBSG7UhhSXe1yCFryZD9B9D398ewSTAfBgNVHSMEGDAWgBTRUE1SLhAmJgUnzM/6\n"\
"PZOaXgIbIDAKBggqhkjOPQQDAgNIADBFAiEAh4e0yeiHaX0YF33vATtlO02dqyQn\n"\
"MBrz8zTOE75pDmECIFFXU9knny26O3fPDYaaE2vA73xxmpkQsqB+S5IE3ioO\n"\
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
"MIIBxzCCAW6gAwIBAgIQYP4Un+GOEaCKdFqUY09YgTAKBggqhkjOPQQDAjAwMRQw\n"\
"EgYDVQQKDAtFeGFtcGxlIEluYzEYMBYGA1UEAwwPRXhhbXBsZSBSb290IENBMB4X\n"\
"DTIwMTAzMTA1MjEwNVoXDTMwMTAzMTA1MjEwNVowNDEUMBIGA1UECgwLRXhhbXBs\n"\
"ZSBJbmMxHDAaBgNVBAMME0V4YW1wbGUgU2lnbmVyIEZGRkYwWTATBgcqhkjOPQIB\n"\
"BggqhkjOPQMBBwNCAARQh54+wS0AriVoVS3oskbT6WNkO+A87IE1G7/56TeNl0t0\n"\
"TZPRDeug/cIv4LweaxjXvTJ1VmbKNm8UIQlbO5bCo2YwZDASBgNVHRMBAf8ECDAG\n"\
"AQH/AgEAMA4GA1UdDwEB/wQEAwIBhjAdBgNVHQ4EFgQU0VBNUi4QJiYFJ8zP+j2T\n"\
"ml4CGyAwHwYDVR0jBBgwFoAUo8ywMj4wp7fCWvdiq273QixAf0swCgYIKoZIzj0E\n"\
"AwIDRwAwRAIgJ7smQM1SZFTKv5pxi3saHtukp+4RRudnodrhYdXf0jMCICm34m9/\n"\
"YIE5M+jATDz7YyPHJqrJsxsfzj5FmQAYPC9r\n"\
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

