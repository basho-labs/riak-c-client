#!/bin/sh

rm -rf certificate


# Generated:
# private key in private/cakey.pem
# root CA in cacert.pem (distribute to clients)


#A private key in key.pem
#A certificate in cert.pem
#A combined private key and certificate in key-cert.pem

rm -rf certificate
mkdir certificate
cd certificate
mkdir newcerts private

echo '01' > serialno 
touch mainindex.txt
cat << EOF > openssl.cnf
# 
# OpenSSL configuration file. 
# 
# Establish working directory. 
dir = ./

[ ca ] 
default_ca = CA_default 

[ CA_default ] 
serial = serialno 
database = mainindex.txt 
new_certs_dir = newcerts 
certificate = cacert.pem 
private_key = private/cakey.pem 
default_days = 1000
default_md = md5 
preserve = no 
email_in_dn = no 
nameopt = default_ca 
certopt = default_ca 
policy = policy_match 

[ policy_match ] 
countryName = match 
stateOrProvinceName = match 
organizationName = match 
organizationalUnitName = optional 
commonName = supplied 
emailAddress = optional  

[ req ] 
default_bits = 1024 # Size of keys 
default_keyfile = key.pem # name of generated keys 
default_md = md5 # message digest algorithm 
string_mask = nombstr # permitted characters 
distinguished_name = req_distinguished_name 
req_extensions = v3_req

[ req_distinguished_name ] 
# Variable name   Prompt string 
#----------------------   ---------------------------------- 
0.organizationName = MyOrg
organizationalUnitName = MyOrgUnit
emailAddress = null@basho.com
emailAddress_max = 40 
localityName = Denver
stateOrProvinceName = CO
countryName = US
countryName_min = 2 
countryName_max = 2 
commonName = TestingCerts
commonName_max = 64 

# Default values for the above, for consistency and less typing. 
# Variable name   Value 
#------------------------------   ------------------------------ 
0.organizationName_default = The Sample Company 
localityName_default = Metropolis 
stateOrProvinceName_default = New York 
countryName_default = US 

[ v3_ca ] 
basicConstraints = CA:TRUE 
subjectKeyIdentifier = hash 
authorityKeyIdentifier = keyid:always,issuer:always

[ v3_req ] 
basicConstraints = CA:FALSE 
subjectKeyIdentifier = hash
EOF


openssl req -new -x509 -extensions v3_ca -keyout private/cakey.pem \
  -subj "/C=US/ST=CO/L=Denver/O=TestOrg/CN=www.example.com" \
  -out cacert.pem -passout pass:foobar123 -days 3650 -config ./openssl.cnf

openssl req -new -nodes -out req.pem -config ./openssl.cnf \
  -subj "/C=US/ST=CO/L=Denver/O=TestOrg/CN=www.example.com"

openssl ca -batch -out cert.pem -config ./openssl.cnf -passin pass:foobar123 -infiles req.pem
cat key.pem cert.pem > key-cert.pem

