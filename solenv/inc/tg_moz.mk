#**************************************************************
#  
#  Licensed to the Apache Software Foundation (ASF) under one
#  or more contributor license agreements.  See the NOTICE file
#  distributed with this work for additional information
#  regarding copyright ownership.  The ASF licenses this file
#  to you under the Apache License, Version 2.0 (the
#  "License"); you may not use this file except in compliance
#  with the License.  You may obtain a copy of the License at
#  
#    http://www.apache.org/licenses/LICENSE-2.0
#  
#  Unless required by applicable law or agreed to in writing,
#  software distributed under the License is distributed on an
#  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
#  KIND, either express or implied.  See the License for the
#  specific language governing permissions and limitations
#  under the License.
#  
#**************************************************************




.IF "$(EXPORTS)"!=""
EXPORT_HEADER=export_mozilla_header
.ENDIF

.IF "$(XPIDLSRCS)"!=""
XPIDL_TYPELIB_MODULE=$(MISC)/$(XPIDL_MODULE).xpt
XPIDLHDR:=$(XPIDLSRCS:s/.idl/.h/)
XPIDLXPT:=$(XPIDLSRCS:s/.idl/.xpt/)
EXTRA_MOZ_TARGET+= $(foreach,i,$(XPIDLHDR) $(INCCOM)/$i)
EXTRA_MOZ_TARGET+= $(foreach,i,$(XPIDLXPT) $(MISC)/$i)

EXTRA_MOZ_TARGET+= $(XPIDL_TYPELIB_MODULE)
.ENDIF

ALLMOZ:=$(EXTRA_MOZ_TARGET) $(DO_XPIDL) $(GEN_PRCPUCFG) $(EXPORT_HEADER)

$(INCCOM)/%.h : %.idl
	$(COMMAND_ECHO)$(COPY) $< $(INCCOM)
	$(COMMAND_ECHO)$(XPIDL) -m header -w -I $(SOLARIDLDIR)/mozilla -I$(INCCOM) -I . -o $(INCCOM)/$* $<

$(MISC)/%.xpt : %.idl
	$(COMMAND_ECHO)$(XPIDL) -m typelib -w -I $(SOLARIDLDIR)/mozilla -I$(INCCOM) -I . -o $(MISC)/$* $<

.INCLUDE : target.mk

.IF "$(GUI)"=="WNT"
CPUCFG=_winnt.cfg
.ENDIF

.IF "$(OS)"=="SOLARIS"
CPUCFG=_solaris32.cfg
.ENDIF

.IF "$(OS)"=="LINUX"
CPUCFG=_linux.cfg
.ENDIF

.IF "$(OS)"=="FREEBSD"
CPUCFG=_freebsd.cfg
.ENDIF

.IF "$(OS)"=="NETBSD"
CPUCFG=_netbsd.cfg
.ENDIF

.IF "$(GEN_PRCPUCFG)"!=""
$(GEN_PRCPUCFG): $(PRJ)/pr/include/md/$(CPUCFG) 
	@$(COPY) $(PRJ)/pr/include/md/$(CPUCFG) $@
.ENDIF

.IF "$(EXPORTS)"!=""
$(EXPORT_HEADER):
	@$(COPY) $(EXPORTS) $(INCCOM)
.ENDIF

.IF "$(XPIDLSRCS)"!=""
$(XPIDL_TYPELIB_MODULE): $(foreach,i,$(XPIDLXPT) $(MISC)/$i)
	$(COMMAND_ECHO)$(XPIDL_LINK) $(MISC)/$(XPIDL_MODULE).xpt $^
	@-mkdir $(BIN)/components
	@$(COPY) $@ $(BIN)/components
.ENDIF
