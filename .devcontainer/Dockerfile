FROM centos:latest

# Install dependencies
RUN yum install -y dnf-plugins-core
RUN dnf config-manager --set-enabled powertools

# Install OpenHPC
RUN ARCH=$(uname -m) && \
    OHPC_URL=http://repos.openhpc.community/OpenHPC/2/CentOS_8/${ARCH}/ohpc-release-2-1.el8.${ARCH}.rpm && \
    yum install -y $OHPC_URL
RUN yum install -y openmpi4-gnu9-ohpc lmod-ohpc

# Install git
RUN yum install -y git

# Load OpenMPI modules
# RUN /bin/bash -c "module load gnu9 && module load openmpi4"
