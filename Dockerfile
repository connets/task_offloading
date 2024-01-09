FROM ubuntu:20.04

# Install all necessary packages
RUN apt-get update \
    && apt-get -y --no-install-recommends install software-properties-common \
        && apt-get update \
    && apt-get -y  --no-install-recommends install wget build-essential \
        libzmq3-dev bison flex python3 python3-pip python3-dev git unzip libxerces-c-dev net-tools\
    && rm -rf /var/lib/apt/lists/*

# Clone omnetpp and set working directory
RUN wget -O omnet.tgz https://github.com/omnetpp/omnetpp/releases/download/omnetpp-6.0.1/omnetpp-6.0.1-core.tgz \
    && tar xvfz omnet.tgz \
    && rm omnet.tgz

WORKDIR /omnetpp-6.0.1

# Build omnetpp
RUN bash -c "source ./setenv ; ./configure WITH_OSG=no WITH_QTENV=no PREFER_CLANG=no PREFER_LLD=no; make -j $(nproc)"

# Change working directory
WORKDIR /app/

# Clone and build all necessary frameworks (INET, Veins, Veins_INET)
RUN git clone -b v4.4.1 https://github.com/inet-framework/inet.git inet \
    && (wget http://veins.car2x.org/download/veins-5.2.zip && unzip veins-5.2.zip && rm veins-5.2.zip) \
    && bash -c "source /omnetpp-6.0.1/setenv && source inet/setenv \
        && (cd inet && make makefiles && make MODE=release all -j$(nproc)) \
        && (cd veins-veins-5.2 && make makefiles && make MODE=release all -j$(nproc)) \
        && (cd veins-veins-5.2/subprojects/veins_inet && make makefiles && make MODE=release all -j$(nproc))"

# Install and build Sumo from src for compatibility with Veins and Veins_INET
RUN apt-get -y --no-install-recommends install cmake g++ libxerces-c-dev libfox-1.6-dev libgdal-dev libproj-dev libgl2ps-dev swig  \
    && wget https://sumo.dlr.de/releases/1.17.0/sumo-src-1.17.0.tar.gz \
    && tar xvzf sumo-src-1.17.0.tar.gz \
    && (cd sumo-1.17.0 && export SUMO_HOME=$(pwd) && cmake -B build . && cmake --build build -j$(nproc)) \
    && cd ..

# Export Sumo env variables
ENV PATH="$PATH:/app/sumo-1.17.0/bin"
ENV SUMO_HOME='/app/sumo-1.17.0'

COPY . /app/task_offloading

RUN bash -c "source /omnetpp-6.0.1/setenv && (cd task_offloading && make makefiles && make MODE=release -j$(nproc))"

COPY launcher.sh /app

CMD bash launcher.sh
