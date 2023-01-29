FROM ubuntu:20.04

RUN apt-get update \
    && apt-get -y --no-install-recommends install software-properties-common \
	&& add-apt-repository ppa:sumo/stable \
	&& apt-get update \
    && apt-get -y  --no-install-recommends install wget build-essential \
        libzmq3-dev bison flex python3 python3-pip git sumo sumo-tools unzip libxerces-c-dev net-tools\  
    && rm -rf /var/lib/apt/lists/*

RUN wget -O omnet.tgz https://github.com/omnetpp/omnetpp/releases/download/omnetpp-6.0.1/omnetpp-6.0.1-core.tgz \
    && tar -xvfz omnet.tgz \
    && rm omnet.tgz 

WORKDIR /omnetpp-6.0.1

RUN bash -c "source ./setenv ; ./configure WITH_OSG=no WITH_QTENV=no PREFER_CLANG=no PREFER_LLD=no; make -j $(nproc)"

WORKDIR /app/

RUN git clone -b v4.4.1 https://github.com/inet-framework/inet.git inet \
    && (wget http://veins.car2x.org/download/veins-5.2.zip && unzip veins-5.2.zip && rm veins-5.2.zip) \
    && bash -c "source /omnetpp-6.0.1/setenv && source inet/setenv \
        && (cd inet && make makefiles && make MODE=release all -j$(nproc)) \
        && (cd veins-veins-5.2 && make makefiles && make MODE=release all -j$(nproc))"

COPY . /app/task_offloading

RUN bash -c "source /omnetpp-6.0.1/setenv && (cd task_offloading && make makefiles && make MODE=release -j$(nproc))"

COPY --chmod=0755 launcher.sh /app

CMD ./launcher.sh
