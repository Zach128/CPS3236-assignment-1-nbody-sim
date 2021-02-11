FROM centos:latest

ARG USERNAME=vscode

# Add default user named vscode
RUN useradd -ms /bin/bash $USERNAME

# Install dependencies
RUN yum install -y dnf-plugins-core
RUN dnf config-manager --set-enabled powertools

# Install OpenHPC
RUN ARCH=$(uname -m) && \
    OHPC_URL=http://repos.openhpc.community/OpenHPC/2/CentOS_8/${ARCH}/ohpc-release-2-1.el8.${ARCH}.rpm && \
    yum install -y $OHPC_URL
RUN yum install -y openmpi4-gnu9-ohpc lmod-ohpc

# Install other dev tools
RUN yum install -y git make gdb cmake valgrind

# Install python
RUN yum install -y python3 python3-tkinter
RUN python3 -m pip install pysimplegui

RUN SNIPPET="export PROMPT_COMMAND='history -a' && export HISTFILE=/commandhistory/.bash_history" \
    && mkdir /commandhistory \
    && touch /commandhistory/.bash_history \
    && chown -R $USERNAME /commandhistory \
    && echo $SNIPPET >> "/home/$USERNAME/.bashrc"

ADD load_mpi.sh /etc/profile.d/

USER $USERNAME

# Load OpenMPI modules
# RUN /bin/bash -c "module load gnu9 && module load openmpi4"
