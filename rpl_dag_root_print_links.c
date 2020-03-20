void
rpl_dag_root_print_links(const char *str)
{
  if(rpl_dag_root_is_root()) {
    if(uip_sr_num_nodes() > 0) {
      uip_sr_node_t *link;
      /* Our routing links */
      LOG_INFO("links: %u routing links in total (%s)\n", uip_sr_num_nodes(), str);
      link = uip_sr_node_head();
      while(link != NULL) {
        char buf[100];
        uip_sr_link_snprint(buf, sizeof(buf), link);
        LOG_INFO("links: %s\n", buf);
        link = uip_sr_node_next(link);
      }
      LOG_INFO("links: end of list\n");
    } else {
      LOG_INFO("No routing links\n");
    }
  }
}
