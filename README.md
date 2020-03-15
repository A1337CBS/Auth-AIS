# AIS_CAESAR - Providing Broadcast Authentication for Vessels Communications (Proof of Concept)
CAESAR is a Broadcast Authentication protocol specifically designed to meet the features and bandwidth constraints of the Automatic Identification System (AIS) communication technology. It has been designed as a standard-compliant AIS application, that can be installed by Class-A and Class-B AIS transceivers to establish secure pairwise session keys with neighboring entities, being them either vessels or port authorities. 


A Proof of Concept using GNURadio and Ettus Research X310 SDRs on how to set up broadcast authentication between two AIS transceivers. It supports different security levels: none, <i>1, 2, 3,4,5 </i> and <i>6</i> that can support different scenarios that a maritime vessel could require.

<p align="center">
     <img alt="ais_tranceiver_flowgraph" src="./images/key-agr.png" width="500">
</p>

<i>For further details, please refer to the paper.</i>

# Why create this?
Nobody has done it in a way that is standard compliant or requires just a software update to make a security service work on AIS. In theory two friendly ships can implement all of this before leaving harbour and then communicate in secrecy.

# How to use
This project has two parts, two C++ programs and a flowgraph in GNURadio. To set them up: </br>
1) Install <i>gnuradio</i> software <br />
2) Install <i>gr-aistx_with_input</i> block to gnuradio (instructions inside the block folder on how to compile and install it). If you are using PyBOMBS, please initialize your environment first. <br />
3) Open <i>ais_transceiever.grc</i> flowgraph in GNURadio.  <br />
4) Make sure ports <b>51999</b> and <b>5200</b> are not adopted by any network protocol. <br />
5) Execute main or compile code from the source.<br />

<p align="center">
     <img alt="ais_tranceiver_flowgraph" src="./images/ais_tranceiver_flowgraph.png" width="500">
</p>

# How to compile code
To compile from source or use a different security level:
```
    g++ -O2 main.cpp -DSECURITY_LEVEL=1 ./secure_ais_protocol.cpp ./ais_receiver/*.c core-master/cpp/core.a -o main
```
## Security Level and other Flags
In order to set a different security level, you can add flag <i>-DSECURITY_LEVEL=<b>t</b></i> that ranges from 0 to 4.

<table>
  <tr>
    <th><b>Security Level (bits)</b></th>
    <th><i><b>t</b></i></th>
  </tr>
  <tr>
    <td>80</td>
    <td>1</td>
  </tr>
  <tr>
    <td>128</td>
    <td>2</td>
  </tr>
  <tr>
    <td>192</td>
    <td>3</td>
  </tr>
  <tr>
    <td>256</td>
    <td>4</td>
  </tr>
</table>

Other flags include: <br />
    -DPORT_SEND or -DPORT_RECEIVE to set another port for send/receive sockets <br />
    -DGEN_KEYS = true or false to set whether to generate keys or not <br />

# Formal verification with ProVerif
The security properties of SecureAIS have been verified formally and experimentally by using the open-source tool ProVerif, demonstrating enhanced security protection with respect to state-of-the-art approaches.

In order to test the security properties, download the file <a href="sais.pv">sais.pv</a> and run: `proverif sais.pv | grep "RESULT"`.

# Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

# Developers
- Ahmed Aziz             (<aaziz@mail.hbku.edu.qa>)<br />
- Pietro Tedeschi        (<ptedeschi@mail.hbku.edu.qa>)<br />
- Savio Sciancalepore    (<ssciancalepore@hbku.edu.qa>)<br />
- Roberto Di Pietro      (<rdipietro@hbku.edu.qa>)<br />
Division of Information and Computing Technology (ICT), College of Science and Engineering (CSE)<br />Hamad Bin Khalifa University (HBKU), Doha, Qatar<br />

# Credits
Credits go to the original authors of TESLA protocol, MIRACL Core Crypto library, gr_aistx and ais_receiver whose original efforts made this possible:
<br />
https://github.com/miracl/core  <br />
https://github.com/trendmicro/ais   <br />
https://github.com/juan0fran/ais_rx <br />

## License
CAESAR is released under the BSD 3-Clause <a href="LICENSE">license</a>.
