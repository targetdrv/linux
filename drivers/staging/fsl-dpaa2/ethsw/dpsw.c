// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright 2014-2016 Freescale Semiconductor Inc.
 * Copyright 2017-2018 NXP
 *
 */

#include <linux/fsl/mc.h>
#include "dpsw.h"
#include "dpsw-cmd.h"

static void build_if_id_bitmap(__le64 *bmap,
			       const u16 *id,
			       const u16 num_ifs)
{
	int i;

	for (i = 0; (i < num_ifs) && (i < DPSW_MAX_IF); i++) {
		if (id[i] < DPSW_MAX_IF)
			bmap[id[i] / 64] |= cpu_to_le64(BIT_MASK(id[i] % 64));
	}
}

/**
 * dpsw_open() - Open a control session for the specified object
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @dpsw_id:	DPSW unique ID
 * @token:	Returned token; use in subsequent API calls
 *
 * This function can be used to open a control session for an
 * already created object; an object may have been declared in
 * the DPL or by calling the dpsw_create() function.
 * This function returns a unique authentication token,
 * associated with the specific object ID and the specific MC
 * portal; this token must be used in all subsequent commands for
 * this specific object
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpsw_open(struct fsl_mc_io *mc_io,
	      u32 cmd_flags,
	      int dpsw_id,
	      u16 *token)
{
	struct fsl_mc_command cmd = { 0 };
	struct dpsw_cmd_open *cmd_params;
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPSW_CMDID_OPEN,
					  cmd_flags,
					  0);
	cmd_params = (struct dpsw_cmd_open *)cmd.params;
	cmd_params->dpsw_id = cpu_to_le32(dpsw_id);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	*token = mc_cmd_hdr_read_token(&cmd);

	return 0;
}

/**
 * dpsw_close() - Close the control session of the object
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPSW object
 *
 * After this function is called, no further operations are
 * allowed on the object without opening a new control session.
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpsw_close(struct fsl_mc_io *mc_io,
	       u32 cmd_flags,
	       u16 token)
{
	struct fsl_mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPSW_CMDID_CLOSE,
					  cmd_flags,
					  token);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

/**
 * dpsw_enable() - Enable DPSW functionality
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPSW object
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_enable(struct fsl_mc_io *mc_io,
		u32 cmd_flags,
		u16 token)
{
	struct fsl_mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPSW_CMDID_ENABLE,
					  cmd_flags,
					  token);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

/**
 * dpsw_disable() - Disable DPSW functionality
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPSW object
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_disable(struct fsl_mc_io *mc_io,
		 u32 cmd_flags,
		 u16 token)
{
	struct fsl_mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPSW_CMDID_DISABLE,
					  cmd_flags,
					  token);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

/**
 * dpsw_reset() - Reset the DPSW, returns the object to initial state.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPSW object
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpsw_reset(struct fsl_mc_io *mc_io,
	       u32 cmd_flags,
	       u16 token)
{
	struct fsl_mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPSW_CMDID_RESET,
					  cmd_flags,
					  token);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

/**
 * dpsw_set_irq_enable() - Set overall interrupt state.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPCI object
 * @irq_index:	The interrupt index to configure
 * @en:		Interrupt state - enable = 1, disable = 0
 *
 * Allows GPP software to control when interrupts are generated.
 * Each interrupt can have up to 32 causes.  The enable/disable control's the
 * overall interrupt state. if the interrupt is disabled no causes will cause
 * an interrupt
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpsw_set_irq_enable(struct fsl_mc_io *mc_io,
			u32 cmd_flags,
			u16 token,
			u8 irq_index,
			u8 en)
{
	struct fsl_mc_command cmd = { 0 };
	struct dpsw_cmd_set_irq_enable *cmd_params;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPSW_CMDID_SET_IRQ_ENABLE,
					  cmd_flags,
					  token);
	cmd_params = (struct dpsw_cmd_set_irq_enable *)cmd.params;
	dpsw_set_field(cmd_params->enable_state, ENABLE, en);
	cmd_params->irq_index = irq_index;

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

/**
 * dpsw_set_irq_mask() - Set interrupt mask.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPCI object
 * @irq_index:	The interrupt index to configure
 * @mask:	Event mask to trigger interrupt;
 *		each bit:
 *			0 = ignore event
 *			1 = consider event for asserting IRQ
 *
 * Every interrupt can have up to 32 causes and the interrupt model supports
 * masking/unmasking each cause independently
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpsw_set_irq_mask(struct fsl_mc_io *mc_io,
		      u32 cmd_flags,
		      u16 token,
		      u8 irq_index,
		      u32 mask)
{
	struct fsl_mc_command cmd = { 0 };
	struct dpsw_cmd_set_irq_mask *cmd_params;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPSW_CMDID_SET_IRQ_MASK,
					  cmd_flags,
					  token);
	cmd_params = (struct dpsw_cmd_set_irq_mask *)cmd.params;
	cmd_params->mask = cpu_to_le32(mask);
	cmd_params->irq_index = irq_index;

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

/**
 * dpsw_get_irq_status() - Get the current status of any pending interrupts
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPSW object
 * @irq_index:	The interrupt index to configure
 * @status:	Returned interrupts status - one bit per cause:
 *			0 = no interrupt pending
 *			1 = interrupt pending
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpsw_get_irq_status(struct fsl_mc_io *mc_io,
			u32 cmd_flags,
			u16 token,
			u8 irq_index,
			u32 *status)
{
	struct fsl_mc_command cmd = { 0 };
	struct dpsw_cmd_get_irq_status *cmd_params;
	struct dpsw_rsp_get_irq_status *rsp_params;
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPSW_CMDID_GET_IRQ_STATUS,
					  cmd_flags,
					  token);
	cmd_params = (struct dpsw_cmd_get_irq_status *)cmd.params;
	cmd_params->status = cpu_to_le32(*status);
	cmd_params->irq_index = irq_index;

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	rsp_params = (struct dpsw_rsp_get_irq_status *)cmd.params;
	*status = le32_to_cpu(rsp_params->status);

	return 0;
}

/**
 * dpsw_clear_irq_status() - Clear a pending interrupt's status
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPCI object
 * @irq_index:	The interrupt index to configure
 * @status:	bits to clear (W1C) - one bit per cause:
 *			0 = don't change
 *			1 = clear status bit
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpsw_clear_irq_status(struct fsl_mc_io *mc_io,
			  u32 cmd_flags,
			  u16 token,
			  u8 irq_index,
			  u32 status)
{
	struct fsl_mc_command cmd = { 0 };
	struct dpsw_cmd_clear_irq_status *cmd_params;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPSW_CMDID_CLEAR_IRQ_STATUS,
					  cmd_flags,
					  token);
	cmd_params = (struct dpsw_cmd_clear_irq_status *)cmd.params;
	cmd_params->status = cpu_to_le32(status);
	cmd_params->irq_index = irq_index;

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

/**
 * dpsw_get_attributes() - Retrieve DPSW attributes
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPSW object
 * @attr:	Returned DPSW attributes
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_get_attributes(struct fsl_mc_io *mc_io,
			u32 cmd_flags,
			u16 token,
			struct dpsw_attr *attr)
{
	struct fsl_mc_command cmd = { 0 };
	struct dpsw_rsp_get_attr *rsp_params;
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPSW_CMDID_GET_ATTR,
					  cmd_flags,
					  token);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	rsp_params = (struct dpsw_rsp_get_attr *)cmd.params;
	attr->num_ifs = le16_to_cpu(rsp_params->num_ifs);
	attr->max_fdbs = rsp_params->max_fdbs;
	attr->num_fdbs = rsp_params->num_fdbs;
	attr->max_vlans = le16_to_cpu(rsp_params->max_vlans);
	attr->num_vlans = le16_to_cpu(rsp_params->num_vlans);
	attr->max_fdb_entries = le16_to_cpu(rsp_params->max_fdb_entries);
	attr->fdb_aging_time = le16_to_cpu(rsp_params->fdb_aging_time);
	attr->id = le32_to_cpu(rsp_params->dpsw_id);
	attr->mem_size = le16_to_cpu(rsp_params->mem_size);
	attr->max_fdb_mc_groups = le16_to_cpu(rsp_params->max_fdb_mc_groups);
	attr->max_meters_per_if = rsp_params->max_meters_per_if;
	attr->options = le64_to_cpu(rsp_params->options);
	attr->component_type = dpsw_get_field(rsp_params->component_type,
					      COMPONENT_TYPE);

	return 0;
}

/**
 * dpsw_if_get_attributes() - Function obtains attributes of interface
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPSW object
 * @if_id:	Interface Identifier
 * @attr:	Returned interface attributes
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_get_attributes(struct fsl_mc_io *mc_io,
			   u32 cmd_flags,
			   u16 token,
			   u16 if_id,
			   struct dpsw_if_attr *attr)
{
	struct dpsw_rsp_if_get_attr *rsp_params;
	struct dpsw_cmd_if *cmd_params;
	struct fsl_mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPSW_CMDID_IF_GET_ATTR,
					  cmd_flags,
					  token);
	cmd_params = (struct dpsw_cmd_if *)cmd.params;
	cmd_params->if_id = cpu_to_le16(if_id);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	rsp_params = (struct dpsw_rsp_if_get_attr *)cmd.params;
	attr->num_tcs = rsp_params->num_tcs;
	attr->rate = le32_to_cpu(rsp_params->rate);
	attr->options = le32_to_cpu(rsp_params->options);
	attr->qdid = le16_to_cpu(rsp_params->qdid);
	attr->enabled = dpsw_get_field(rsp_params->conf, ENABLE);
	attr->accept_all_vlan = dpsw_get_field(rsp_params->conf,
					       ACCEPT_ALL_VLAN);
	attr->admit_untagged = dpsw_get_field(rsp_params->conf,
					      ADMIT_UNTAGGED);

	return 0;
}

/**
 * dpsw_if_set_link_cfg() - Set the link configuration.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPSW object
 * @if_id:	Interface id
 * @cfg:	Link configuration
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpsw_if_set_link_cfg(struct fsl_mc_io *mc_io,
			 u32 cmd_flags,
			 u16 token,
			 u16 if_id,
			 struct dpsw_link_cfg *cfg)
{
	struct fsl_mc_command cmd = { 0 };
	struct dpsw_cmd_if_set_link_cfg *cmd_params;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPSW_CMDID_IF_SET_LINK_CFG,
					  cmd_flags,
					  token);
	cmd_params = (struct dpsw_cmd_if_set_link_cfg *)cmd.params;
	cmd_params->if_id = cpu_to_le16(if_id);
	cmd_params->rate = cpu_to_le32(cfg->rate);
	cmd_params->options = cpu_to_le64(cfg->options);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

/**
 * dpsw_if_get_link_state - Return the link state
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPSW object
 * @if_id:	Interface id
 * @state:	Link state	1 - linkup, 0 - link down or disconnected
 *
 * @Return	'0' on Success; Error code otherwise.
 */
int dpsw_if_get_link_state(struct fsl_mc_io *mc_io,
			   u32 cmd_flags,
			   u16 token,
			   u16 if_id,
			   struct dpsw_link_state *state)
{
	struct fsl_mc_command cmd = { 0 };
	struct dpsw_cmd_if_get_link_state *cmd_params;
	struct dpsw_rsp_if_get_link_state *rsp_params;
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPSW_CMDID_IF_GET_LINK_STATE,
					  cmd_flags,
					  token);
	cmd_params = (struct dpsw_cmd_if_get_link_state *)cmd.params;
	cmd_params->if_id = cpu_to_le16(if_id);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	rsp_params = (struct dpsw_rsp_if_get_link_state *)cmd.params;
	state->rate = le32_to_cpu(rsp_params->rate);
	state->options = le64_to_cpu(rsp_params->options);
	state->up = dpsw_get_field(rsp_params->up, UP);

	return 0;
}

/**
 * dpsw_if_set_flooding() - Enable Disable flooding for particular interface
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPSW object
 * @if_id:	Interface Identifier
 * @en:		1 - enable, 0 - disable
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_set_flooding(struct fsl_mc_io *mc_io,
			 u32 cmd_flags,
			 u16 token,
			 u16 if_id,
			 u8 en)
{
	struct fsl_mc_command cmd = { 0 };
	struct dpsw_cmd_if_set_flooding *cmd_params;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPSW_CMDID_IF_SET_FLOODING,
					  cmd_flags,
					  token);
	cmd_params = (struct dpsw_cmd_if_set_flooding *)cmd.params;
	cmd_params->if_id = cpu_to_le16(if_id);
	dpsw_set_field(cmd_params->enable, ENABLE, en);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

/**
 * dpsw_if_set_broadcast() - Enable/disable broadcast for particular interface
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPSW object
 * @if_id:	Interface Identifier
 * @en:		1 - enable, 0 - disable
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_set_broadcast(struct fsl_mc_io *mc_io,
			  u32 cmd_flags,
			  u16 token,
			  u16 if_id,
			  u8 en)
{
	struct fsl_mc_command cmd = { 0 };
	struct dpsw_cmd_if_set_broadcast *cmd_params;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPSW_CMDID_IF_SET_BROADCAST,
					  cmd_flags,
					  token);
	cmd_params = (struct dpsw_cmd_if_set_broadcast *)cmd.params;
	cmd_params->if_id = cpu_to_le16(if_id);
	dpsw_set_field(cmd_params->enable, ENABLE, en);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

/**
 * dpsw_if_set_tci() - Set default VLAN Tag Control Information (TCI)
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPSW object
 * @if_id:	Interface Identifier
 * @cfg:	Tag Control Information Configuration
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_set_tci(struct fsl_mc_io *mc_io,
		    u32 cmd_flags,
		    u16 token,
		    u16 if_id,
		    const struct dpsw_tci_cfg *cfg)
{
	struct fsl_mc_command cmd = { 0 };
	struct dpsw_cmd_if_set_tci *cmd_params;
	u16 tmp_conf = 0;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPSW_CMDID_IF_SET_TCI,
					  cmd_flags,
					  token);
	cmd_params = (struct dpsw_cmd_if_set_tci *)cmd.params;
	cmd_params->if_id = cpu_to_le16(if_id);
	dpsw_set_field(tmp_conf, VLAN_ID, cfg->vlan_id);
	dpsw_set_field(tmp_conf, DEI, cfg->dei);
	dpsw_set_field(tmp_conf, PCP, cfg->pcp);
	cmd_params->conf = cpu_to_le16(tmp_conf);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

/**
 * dpsw_if_get_tci() - Get default VLAN Tag Control Information (TCI)
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPSW object
 * @if_id:	Interface Identifier
 * @cfg:	Tag Control Information Configuration
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_get_tci(struct fsl_mc_io *mc_io,
		    u32 cmd_flags,
		    u16 token,
		    u16 if_id,
		    struct dpsw_tci_cfg *cfg)
{
	struct fsl_mc_command cmd = { 0 };
	struct dpsw_cmd_if_get_tci *cmd_params;
	struct dpsw_rsp_if_get_tci *rsp_params;
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPSW_CMDID_IF_GET_TCI,
					  cmd_flags,
					  token);
	cmd_params = (struct dpsw_cmd_if_get_tci *)cmd.params;
	cmd_params->if_id = cpu_to_le16(if_id);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	rsp_params = (struct dpsw_rsp_if_get_tci *)cmd.params;
	cfg->pcp = rsp_params->pcp;
	cfg->dei = rsp_params->dei;
	cfg->vlan_id = le16_to_cpu(rsp_params->vlan_id);

	return 0;
}

/**
 * dpsw_if_set_stp() - Function sets Spanning Tree Protocol (STP) state.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPSW object
 * @if_id:	Interface Identifier
 * @cfg:	STP State configuration parameters
 *
 * The following STP states are supported -
 * blocking, listening, learning, forwarding and disabled.
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_set_stp(struct fsl_mc_io *mc_io,
		    u32 cmd_flags,
		    u16 token,
		    u16 if_id,
		    const struct dpsw_stp_cfg *cfg)
{
	struct fsl_mc_command cmd = { 0 };
	struct dpsw_cmd_if_set_stp *cmd_params;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPSW_CMDID_IF_SET_STP,
					  cmd_flags,
					  token);
	cmd_params = (struct dpsw_cmd_if_set_stp *)cmd.params;
	cmd_params->if_id = cpu_to_le16(if_id);
	cmd_params->vlan_id = cpu_to_le16(cfg->vlan_id);
	dpsw_set_field(cmd_params->state, STATE, cfg->state);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

/**
 * dpsw_if_get_counter() - Get specific counter of particular interface
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPSW object
 * @if_id:	Interface Identifier
 * @type:	Counter type
 * @counter:	return value
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_get_counter(struct fsl_mc_io *mc_io,
			u32 cmd_flags,
			u16 token,
			u16 if_id,
			enum dpsw_counter type,
			u64 *counter)
{
	struct fsl_mc_command cmd = { 0 };
	struct dpsw_cmd_if_get_counter *cmd_params;
	struct dpsw_rsp_if_get_counter *rsp_params;
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPSW_CMDID_IF_GET_COUNTER,
					  cmd_flags,
					  token);
	cmd_params = (struct dpsw_cmd_if_get_counter *)cmd.params;
	cmd_params->if_id = cpu_to_le16(if_id);
	dpsw_set_field(cmd_params->type, COUNTER_TYPE, type);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	rsp_params = (struct dpsw_rsp_if_get_counter *)cmd.params;
	*counter = le64_to_cpu(rsp_params->counter);

	return 0;
}

/**
 * dpsw_if_enable() - Enable Interface
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPSW object
 * @if_id:	Interface Identifier
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_enable(struct fsl_mc_io *mc_io,
		   u32 cmd_flags,
		   u16 token,
		   u16 if_id)
{
	struct fsl_mc_command cmd = { 0 };
	struct dpsw_cmd_if *cmd_params;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPSW_CMDID_IF_ENABLE,
					  cmd_flags,
					  token);
	cmd_params = (struct dpsw_cmd_if *)cmd.params;
	cmd_params->if_id = cpu_to_le16(if_id);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

/**
 * dpsw_if_disable() - Disable Interface
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPSW object
 * @if_id:	Interface Identifier
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_disable(struct fsl_mc_io *mc_io,
		    u32 cmd_flags,
		    u16 token,
		    u16 if_id)
{
	struct fsl_mc_command cmd = { 0 };
	struct dpsw_cmd_if *cmd_params;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPSW_CMDID_IF_DISABLE,
					  cmd_flags,
					  token);
	cmd_params = (struct dpsw_cmd_if *)cmd.params;
	cmd_params->if_id = cpu_to_le16(if_id);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

/**
 * dpsw_if_set_max_frame_length() - Set Maximum Receive frame length.
 * @mc_io:		Pointer to MC portal's I/O object
 * @cmd_flags:		Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:		Token of DPSW object
 * @if_id:		Interface Identifier
 * @frame_length:	Maximum Frame Length
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_set_max_frame_length(struct fsl_mc_io *mc_io,
				 u32 cmd_flags,
				 u16 token,
				 u16 if_id,
				 u16 frame_length)
{
	struct fsl_mc_command cmd = { 0 };
	struct dpsw_cmd_if_set_max_frame_length *cmd_params;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPSW_CMDID_IF_SET_MAX_FRAME_LENGTH,
					  cmd_flags,
					  token);
	cmd_params = (struct dpsw_cmd_if_set_max_frame_length *)cmd.params;
	cmd_params->if_id = cpu_to_le16(if_id);
	cmd_params->frame_length = cpu_to_le16(frame_length);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

/**
 * dpsw_vlan_add() - Adding new VLAN to DPSW.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPSW object
 * @vlan_id:	VLAN Identifier
 * @cfg:	VLAN configuration
 *
 * Only VLAN ID and FDB ID are required parameters here.
 * 12 bit VLAN ID is defined in IEEE802.1Q.
 * Adding a duplicate VLAN ID is not allowed.
 * FDB ID can be shared across multiple VLANs. Shared learning
 * is obtained by calling dpsw_vlan_add for multiple VLAN IDs
 * with same fdb_id
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_vlan_add(struct fsl_mc_io *mc_io,
		  u32 cmd_flags,
		  u16 token,
		  u16 vlan_id,
		  const struct dpsw_vlan_cfg *cfg)
{
	struct fsl_mc_command cmd = { 0 };
	struct dpsw_vlan_add *cmd_params;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPSW_CMDID_VLAN_ADD,
					  cmd_flags,
					  token);
	cmd_params = (struct dpsw_vlan_add *)cmd.params;
	cmd_params->fdb_id = cpu_to_le16(cfg->fdb_id);
	cmd_params->vlan_id = cpu_to_le16(vlan_id);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

/**
 * dpsw_vlan_add_if() - Adding a set of interfaces to an existing VLAN.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPSW object
 * @vlan_id:	VLAN Identifier
 * @cfg:	Set of interfaces to add
 *
 * It adds only interfaces not belonging to this VLAN yet,
 * otherwise an error is generated and an entire command is
 * ignored. This function can be called numerous times always
 * providing required interfaces delta.
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_vlan_add_if(struct fsl_mc_io *mc_io,
		     u32 cmd_flags,
		     u16 token,
		     u16 vlan_id,
		     const struct dpsw_vlan_if_cfg *cfg)
{
	struct fsl_mc_command cmd = { 0 };
	struct dpsw_cmd_vlan_manage_if *cmd_params;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPSW_CMDID_VLAN_ADD_IF,
					  cmd_flags,
					  token);
	cmd_params = (struct dpsw_cmd_vlan_manage_if *)cmd.params;
	cmd_params->vlan_id = cpu_to_le16(vlan_id);
	build_if_id_bitmap(cmd_params->if_id, cfg->if_id, cfg->num_ifs);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

/**
 * dpsw_vlan_add_if_untagged() - Defining a set of interfaces that should be
 *				transmitted as untagged.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPSW object
 * @vlan_id:	VLAN Identifier
 * @cfg:	Set of interfaces that should be transmitted as untagged
 *
 * These interfaces should already belong to this VLAN.
 * By default all interfaces are transmitted as tagged.
 * Providing un-existing interface or untagged interface that is
 * configured untagged already generates an error and the entire
 * command is ignored.
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_vlan_add_if_untagged(struct fsl_mc_io *mc_io,
			      u32 cmd_flags,
			      u16 token,
			      u16 vlan_id,
			      const struct dpsw_vlan_if_cfg *cfg)
{
	struct fsl_mc_command cmd = { 0 };
	struct dpsw_cmd_vlan_manage_if *cmd_params;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPSW_CMDID_VLAN_ADD_IF_UNTAGGED,
					  cmd_flags,
					  token);
	cmd_params = (struct dpsw_cmd_vlan_manage_if *)cmd.params;
	cmd_params->vlan_id = cpu_to_le16(vlan_id);
	build_if_id_bitmap(cmd_params->if_id, cfg->if_id, cfg->num_ifs);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

/**
 * dpsw_vlan_remove_if() - Remove interfaces from an existing VLAN.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPSW object
 * @vlan_id:	VLAN Identifier
 * @cfg:	Set of interfaces that should be removed
 *
 * Interfaces must belong to this VLAN, otherwise an error
 * is returned and an the command is ignored
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_vlan_remove_if(struct fsl_mc_io *mc_io,
			u32 cmd_flags,
			u16 token,
			u16 vlan_id,
			const struct dpsw_vlan_if_cfg *cfg)
{
	struct fsl_mc_command cmd = { 0 };
	struct dpsw_cmd_vlan_manage_if *cmd_params;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPSW_CMDID_VLAN_REMOVE_IF,
					  cmd_flags,
					  token);
	cmd_params = (struct dpsw_cmd_vlan_manage_if *)cmd.params;
	cmd_params->vlan_id = cpu_to_le16(vlan_id);
	build_if_id_bitmap(cmd_params->if_id, cfg->if_id, cfg->num_ifs);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

/**
 * dpsw_vlan_remove_if_untagged() - Define a set of interfaces that should be
 *		converted from transmitted as untagged to transmit as tagged.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPSW object
 * @vlan_id:	VLAN Identifier
 * @cfg:	Set of interfaces that should be removed
 *
 * Interfaces provided by API have to belong to this VLAN and
 * configured untagged, otherwise an error is returned and the
 * command is ignored
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_vlan_remove_if_untagged(struct fsl_mc_io *mc_io,
				 u32 cmd_flags,
				 u16 token,
				 u16 vlan_id,
				 const struct dpsw_vlan_if_cfg *cfg)
{
	struct fsl_mc_command cmd = { 0 };
	struct dpsw_cmd_vlan_manage_if *cmd_params;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPSW_CMDID_VLAN_REMOVE_IF_UNTAGGED,
					  cmd_flags,
					  token);
	cmd_params = (struct dpsw_cmd_vlan_manage_if *)cmd.params;
	cmd_params->vlan_id = cpu_to_le16(vlan_id);
	build_if_id_bitmap(cmd_params->if_id, cfg->if_id, cfg->num_ifs);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

/**
 * dpsw_vlan_remove() - Remove an entire VLAN
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPSW object
 * @vlan_id:	VLAN Identifier
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_vlan_remove(struct fsl_mc_io *mc_io,
		     u32 cmd_flags,
		     u16 token,
		     u16 vlan_id)
{
	struct fsl_mc_command cmd = { 0 };
	struct dpsw_cmd_vlan_remove *cmd_params;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPSW_CMDID_VLAN_REMOVE,
					  cmd_flags,
					  token);
	cmd_params = (struct dpsw_cmd_vlan_remove *)cmd.params;
	cmd_params->vlan_id = cpu_to_le16(vlan_id);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

/**
 * dpsw_fdb_add_unicast() - Function adds an unicast entry into MAC lookup table
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPSW object
 * @fdb_id:	Forwarding Database Identifier
 * @cfg:	Unicast entry configuration
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_fdb_add_unicast(struct fsl_mc_io *mc_io,
			 u32 cmd_flags,
			 u16 token,
			 u16 fdb_id,
			 const struct dpsw_fdb_unicast_cfg *cfg)
{
	struct fsl_mc_command cmd = { 0 };
	struct dpsw_cmd_fdb_unicast_op *cmd_params;
	int i;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPSW_CMDID_FDB_ADD_UNICAST,
					  cmd_flags,
					  token);
	cmd_params = (struct dpsw_cmd_fdb_unicast_op *)cmd.params;
	cmd_params->fdb_id = cpu_to_le16(fdb_id);
	cmd_params->if_egress = cpu_to_le16(cfg->if_egress);
	for (i = 0; i < 6; i++)
		cmd_params->mac_addr[i] = cfg->mac_addr[5 - i];
	dpsw_set_field(cmd_params->type, ENTRY_TYPE, cfg->type);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

/**
 * dpsw_fdb_dump() - Dump the content of FDB table into memory.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPSW object
 * @fdb_id:	Forwarding Database Identifier
 * @iova_addr:	Data will be stored here as an array of struct fdb_dump_entry
 * @iova_size:	Memory size allocated at iova_addr
 * @num_entries:Number of entries written at iova_addr
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 *
 * The memory allocated at iova_addr must be initialized with zero before
 * command execution. If the FDB table does not fit into memory MC will stop
 * after the memory is filled up.
 * The struct fdb_dump_entry array must be parsed until the end of memory
 * area or until an entry with mac_addr set to zero is found.
 */
int dpsw_fdb_dump(struct fsl_mc_io *mc_io,
		  u32 cmd_flags,
		  u16 token,
		  u16 fdb_id,
		  u64 iova_addr,
		  u32 iova_size,
		  u16 *num_entries)
{
	struct dpsw_cmd_fdb_dump *cmd_params;
	struct dpsw_rsp_fdb_dump *rsp_params;
	struct fsl_mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPSW_CMDID_FDB_DUMP,
					  cmd_flags,
					  token);
	cmd_params = (struct dpsw_cmd_fdb_dump *)cmd.params;
	cmd_params->fdb_id = cpu_to_le16(fdb_id);
	cmd_params->iova_addr = cpu_to_le64(iova_addr);
	cmd_params->iova_size = cpu_to_le32(iova_size);

	/* send command to mc */
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	rsp_params = (struct dpsw_rsp_fdb_dump *)cmd.params;
	*num_entries = le16_to_cpu(rsp_params->num_entries);

	return 0;
}

/**
 * dpsw_fdb_remove_unicast() - removes an entry from MAC lookup table
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPSW object
 * @fdb_id:	Forwarding Database Identifier
 * @cfg:	Unicast entry configuration
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_fdb_remove_unicast(struct fsl_mc_io *mc_io,
			    u32 cmd_flags,
			    u16 token,
			    u16 fdb_id,
			    const struct dpsw_fdb_unicast_cfg *cfg)
{
	struct fsl_mc_command cmd = { 0 };
	struct dpsw_cmd_fdb_unicast_op *cmd_params;
	int i;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPSW_CMDID_FDB_REMOVE_UNICAST,
					  cmd_flags,
					  token);
	cmd_params = (struct dpsw_cmd_fdb_unicast_op *)cmd.params;
	cmd_params->fdb_id = cpu_to_le16(fdb_id);
	for (i = 0; i < 6; i++)
		cmd_params->mac_addr[i] = cfg->mac_addr[5 - i];
	cmd_params->if_egress = cpu_to_le16(cfg->if_egress);
	dpsw_set_field(cmd_params->type, ENTRY_TYPE, cfg->type);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

/**
 * dpsw_fdb_add_multicast() - Add a set of egress interfaces to multi-cast group
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPSW object
 * @fdb_id:	Forwarding Database Identifier
 * @cfg:	Multicast entry configuration
 *
 * If group doesn't exist, it will be created.
 * It adds only interfaces not belonging to this multicast group
 * yet, otherwise error will be generated and the command is
 * ignored.
 * This function may be called numerous times always providing
 * required interfaces delta.
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_fdb_add_multicast(struct fsl_mc_io *mc_io,
			   u32 cmd_flags,
			   u16 token,
			   u16 fdb_id,
			   const struct dpsw_fdb_multicast_cfg *cfg)
{
	struct fsl_mc_command cmd = { 0 };
	struct dpsw_cmd_fdb_multicast_op *cmd_params;
	int i;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPSW_CMDID_FDB_ADD_MULTICAST,
					  cmd_flags,
					  token);
	cmd_params = (struct dpsw_cmd_fdb_multicast_op *)cmd.params;
	cmd_params->fdb_id = cpu_to_le16(fdb_id);
	cmd_params->num_ifs = cpu_to_le16(cfg->num_ifs);
	dpsw_set_field(cmd_params->type, ENTRY_TYPE, cfg->type);
	build_if_id_bitmap(cmd_params->if_id, cfg->if_id, cfg->num_ifs);
	for (i = 0; i < 6; i++)
		cmd_params->mac_addr[i] = cfg->mac_addr[5 - i];

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

/**
 * dpsw_fdb_remove_multicast() - Removing interfaces from an existing multicast
 *				group.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPSW object
 * @fdb_id:	Forwarding Database Identifier
 * @cfg:	Multicast entry configuration
 *
 * Interfaces provided by this API have to exist in the group,
 * otherwise an error will be returned and an entire command
 * ignored. If there is no interface left in the group,
 * an entire group is deleted
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_fdb_remove_multicast(struct fsl_mc_io *mc_io,
			      u32 cmd_flags,
			      u16 token,
			      u16 fdb_id,
			      const struct dpsw_fdb_multicast_cfg *cfg)
{
	struct fsl_mc_command cmd = { 0 };
	struct dpsw_cmd_fdb_multicast_op *cmd_params;
	int i;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPSW_CMDID_FDB_REMOVE_MULTICAST,
					  cmd_flags,
					  token);
	cmd_params = (struct dpsw_cmd_fdb_multicast_op *)cmd.params;
	cmd_params->fdb_id = cpu_to_le16(fdb_id);
	cmd_params->num_ifs = cpu_to_le16(cfg->num_ifs);
	dpsw_set_field(cmd_params->type, ENTRY_TYPE, cfg->type);
	build_if_id_bitmap(cmd_params->if_id, cfg->if_id, cfg->num_ifs);
	for (i = 0; i < 6; i++)
		cmd_params->mac_addr[i] = cfg->mac_addr[5 - i];

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

/**
 * dpsw_fdb_set_learning_mode() - Define FDB learning mode
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPSW object
 * @fdb_id:	Forwarding Database Identifier
 * @mode:	Learning mode
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_fdb_set_learning_mode(struct fsl_mc_io *mc_io,
			       u32 cmd_flags,
			       u16 token,
			       u16 fdb_id,
			       enum dpsw_fdb_learning_mode mode)
{
	struct fsl_mc_command cmd = { 0 };
	struct dpsw_cmd_fdb_set_learning_mode *cmd_params;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPSW_CMDID_FDB_SET_LEARNING_MODE,
					  cmd_flags,
					  token);
	cmd_params = (struct dpsw_cmd_fdb_set_learning_mode *)cmd.params;
	cmd_params->fdb_id = cpu_to_le16(fdb_id);
	dpsw_set_field(cmd_params->mode, LEARNING_MODE, mode);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

/**
 * dpsw_get_api_version() - Get Data Path Switch API version
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @major_ver:	Major version of data path switch API
 * @minor_ver:	Minor version of data path switch API
 *
 * Return:  '0' on Success; Error code otherwise.
 */
int dpsw_get_api_version(struct fsl_mc_io *mc_io,
			 u32 cmd_flags,
			 u16 *major_ver,
			 u16 *minor_ver)
{
	struct fsl_mc_command cmd = { 0 };
	struct dpsw_rsp_get_api_version *rsp_params;
	int err;

	cmd.header = mc_encode_cmd_header(DPSW_CMDID_GET_API_VERSION,
					  cmd_flags,
					  0);

	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	rsp_params = (struct dpsw_rsp_get_api_version *)cmd.params;
	*major_ver = le16_to_cpu(rsp_params->version_major);
	*minor_ver = le16_to_cpu(rsp_params->version_minor);

	return 0;
}

/**
 * dpsw_if_get_port_mac_addr()
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPSW object
 * @if_id:	Interface Identifier
 * @mac_addr:	MAC address of the physical port, if any, otherwise 0
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_get_port_mac_addr(struct fsl_mc_io *mc_io, u32 cmd_flags, u16 token,
			      u16 if_id, u8 mac_addr[6])
{
	struct dpsw_rsp_if_get_mac_addr *rsp_params;
	struct fsl_mc_command cmd = { 0 };
	struct dpsw_cmd_if *cmd_params;
	int err, i;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPSW_CMDID_IF_GET_PORT_MAC_ADDR,
					  cmd_flags,
					  token);
	cmd_params = (struct dpsw_cmd_if *)cmd.params;
	cmd_params->if_id = cpu_to_le16(if_id);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	rsp_params = (struct dpsw_rsp_if_get_mac_addr *)cmd.params;
	for (i = 0; i < 6; i++)
		mac_addr[5 - i] = rsp_params->mac_addr[i];

	return 0;
}

/**
 * dpsw_if_get_primary_mac_addr()
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPSW object
 * @if_id:	Interface Identifier
 * @mac_addr:	MAC address of the physical port, if any, otherwise 0
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_get_primary_mac_addr(struct fsl_mc_io *mc_io, u32 cmd_flags,
				 u16 token, u16 if_id, u8 mac_addr[6])
{
	struct dpsw_rsp_if_get_mac_addr *rsp_params;
	struct fsl_mc_command cmd = { 0 };
	struct dpsw_cmd_if *cmd_params;
	int err, i;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPSW_CMDID_IF_SET_PRIMARY_MAC_ADDR,
					  cmd_flags,
					  token);
	cmd_params = (struct dpsw_cmd_if *)cmd.params;
	cmd_params->if_id = cpu_to_le16(if_id);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	rsp_params = (struct dpsw_rsp_if_get_mac_addr *)cmd.params;
	for (i = 0; i < 6; i++)
		mac_addr[5 - i] = rsp_params->mac_addr[i];

	return 0;
}

/**
 * dpsw_if_set_primary_mac_addr()
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPSW object
 * @if_id:	Interface Identifier
 * @mac_addr:	MAC address of the physical port, if any, otherwise 0
 *
 * Return:	Completion status. '0' on Success; Error code otherwise.
 */
int dpsw_if_set_primary_mac_addr(struct fsl_mc_io *mc_io, u32 cmd_flags,
				 u16 token, u16 if_id, u8 mac_addr[6])
{
	struct dpsw_cmd_if_set_mac_addr *cmd_params;
	struct fsl_mc_command cmd = { 0 };
	int i;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPSW_CMDID_IF_SET_PRIMARY_MAC_ADDR,
					  cmd_flags,
					  token);
	cmd_params = (struct dpsw_cmd_if_set_mac_addr *)cmd.params;
	cmd_params->if_id = cpu_to_le16(if_id);
	for (i = 0; i < 6; i++)
		cmd_params->mac_addr[i] = mac_addr[5 - i];

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

/**
 * dpsw_acl_add() - Adds ACL to L2 switch.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPSW object
 * @acl_id:	Returned ACL ID, for the future reference
 * @cfg:	ACL configuration
 *
 * Create Access Control List. Multiple ACLs can be created and
 * co-exist in L2 switch
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpsw_acl_add(struct fsl_mc_io *mc_io,
		 u32 cmd_flags,
		 u16 token,
		 u16 *acl_id,
		 const struct dpsw_acl_cfg  *cfg)
{
	struct dpsw_cmd_acl_add *cmd_params;
	struct dpsw_rsp_acl_add *rsp_params;
	struct fsl_mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPSW_CMDID_ACL_ADD,
					  cmd_flags,
					  token);
	cmd_params = (struct dpsw_cmd_acl_add *)cmd.params;
	cmd_params->max_entries = cpu_to_le16(cfg->max_entries);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	rsp_params = (struct dpsw_rsp_acl_add *)cmd.params;
	*acl_id = le16_to_cpu(rsp_params->acl_id);

	return 0;
}

/**
 * dpsw_acl_remove() - Removes ACL from L2 switch.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPSW object
 * @acl_id:	ACL ID
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpsw_acl_remove(struct fsl_mc_io *mc_io,
		    u32 cmd_flags,
		    u16 token,
		    u16 acl_id)
{
	struct dpsw_cmd_acl_remove *cmd_params;
	struct fsl_mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPSW_CMDID_ACL_REMOVE,
					  cmd_flags,
					  token);
	cmd_params = (struct dpsw_cmd_acl_remove *)cmd.params;
	cmd_params->acl_id = cpu_to_le16(acl_id);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

/**
 * dpsw_acl_add_if() - Associate interface/interfaces with ACL.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPSW object
 * @acl_id:	ACL ID
 * @cfg:	Interfaces list
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpsw_acl_add_if(struct fsl_mc_io *mc_io,
		    u32 cmd_flags,
		    u16 token,
		    u16 acl_id,
		    const struct dpsw_acl_if_cfg *cfg)
{
	struct dpsw_cmd_acl_if *cmd_params;
	struct fsl_mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPSW_CMDID_ACL_ADD_IF,
					  cmd_flags,
					  token);
	cmd_params = (struct dpsw_cmd_acl_if *)cmd.params;
	cmd_params->acl_id = cpu_to_le16(acl_id);
	cmd_params->num_ifs = cpu_to_le16(cfg->num_ifs);
	build_if_id_bitmap(cmd_params->if_id, cfg->if_id, cfg->num_ifs);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

/**
 * dpsw_acl_remove_if() - De-associate interface/interfaces from ACL.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPSW object
 * @acl_id:	ACL ID
 * @cfg:	Interfaces list
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpsw_acl_remove_if(struct fsl_mc_io *mc_io,
		       u32 cmd_flags,
		       u16 token,
		       u16 acl_id,
		       const struct dpsw_acl_if_cfg *cfg)
{
	struct dpsw_cmd_acl_if *cmd_params;
	struct fsl_mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPSW_CMDID_ACL_REMOVE_IF,
					  cmd_flags,
					  token);
	cmd_params = (struct dpsw_cmd_acl_if *)cmd.params;
	cmd_params->acl_id = cpu_to_le16(acl_id);
	cmd_params->num_ifs = cpu_to_le16(cfg->num_ifs);
	build_if_id_bitmap(cmd_params->if_id, cfg->if_id, cfg->num_ifs);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

/**
 * dpsw_acl_prepare_entry_cfg() - Set an entry to ACL.
 * @key:		Key
 * @entry_cfg_buf:	Zeroed 256 bytes of memory before mapping it to DMA
 *
 * This function has to be called before adding or removing acl_entry
 *
 */
void dpsw_acl_prepare_entry_cfg(const struct dpsw_acl_key *key,
				u8 *entry_cfg_buf)
{
	struct dpsw_prep_acl_entry *ext_params;
	int i;

	ext_params = (struct dpsw_prep_acl_entry *)entry_cfg_buf;

	for (i = 0; i < 6; i++) {
		ext_params->match_l2_dest_mac[i] = key->match.l2_dest_mac[5 - i];
		ext_params->match_l2_source_mac[i] = key->match.l2_source_mac[5 - i];
		ext_params->mask_l2_dest_mac[i] = key->mask.l2_dest_mac[5 - i];
		ext_params->mask_l2_source_mac[i] = key->mask.l2_source_mac[5 - i];
	}

	ext_params->match_l2_tpid = cpu_to_le16(key->match.l2_tpid);
	ext_params->match_l2_vlan_id = cpu_to_le16(key->match.l2_vlan_id);
	ext_params->match_l3_dest_ip = cpu_to_le32(key->match.l3_dest_ip);
	ext_params->match_l3_source_ip = cpu_to_le32(key->match.l3_source_ip);
	ext_params->match_l4_dest_port = cpu_to_le16(key->match.l4_dest_port);
	ext_params->match_l4_source_port = cpu_to_le16(key->match.l4_source_port);
	ext_params->match_l2_ether_type = cpu_to_le16(key->match.l2_ether_type);
	ext_params->match_l2_pcp_dei = key->match.l2_pcp_dei;
	ext_params->match_l3_dscp = key->match.l3_dscp;

	ext_params->mask_l2_tpid = cpu_to_le16(key->mask.l2_tpid);
	ext_params->mask_l2_vlan_id = cpu_to_le16(key->mask.l2_vlan_id);
	ext_params->mask_l3_dest_ip = cpu_to_le32(key->mask.l3_dest_ip);
	ext_params->mask_l3_source_ip = cpu_to_le32(key->mask.l3_source_ip);
	ext_params->mask_l4_dest_port = cpu_to_le16(key->mask.l4_dest_port);
	ext_params->mask_l4_source_port = cpu_to_le16(key->mask.l4_source_port);
	ext_params->mask_l2_ether_type = cpu_to_le16(key->mask.l2_ether_type);
	ext_params->mask_l2_pcp_dei = key->mask.l2_pcp_dei;
	ext_params->mask_l3_dscp = key->mask.l3_dscp;
	ext_params->match_l3_protocol = key->match.l3_protocol;
	ext_params->mask_l3_protocol = key->mask.l3_protocol;
	ext_params->match_frame_flags = key->match.frame_flags;
	ext_params->mask_frame_flags = key->mask.frame_flags;
}

/**
 * dpsw_acl_add_entry() - Adds an entry to ACL.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPSW object
 * @acl_id:	ACL ID
 * @cfg:	Entry configuration
 *
 * warning: This function has to be called after dpsw_acl_set_entry_cfg()
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpsw_acl_add_entry(struct fsl_mc_io *mc_io,
		       u32 cmd_flags,
		       u16 token,
		       u16 acl_id,
		       const struct dpsw_acl_entry_cfg *cfg)
{
	struct dpsw_cmd_acl_entry *cmd_params;
	struct fsl_mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPSW_CMDID_ACL_ADD_ENTRY,
					  cmd_flags,
					  token);
	cmd_params = (struct dpsw_cmd_acl_entry *)cmd.params;
	cmd_params->acl_id = cpu_to_le16(acl_id);
	cmd_params->result_if_id = cpu_to_le16(cfg->result.if_id);
	cmd_params->precedence = cpu_to_le32(cfg->precedence);
	cmd_params->key_iova = cpu_to_le64(cfg->key_iova);
	dpsw_set_field(cmd_params->result_action,
		       RESULT_ACTION,
		       cfg->result.action);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}
