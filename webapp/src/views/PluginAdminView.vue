<template>
    <BasePage :title="$t('pluginadmin.PluginSettings')" :isLoading="dataLoading">
        <BootstrapAlert v-model="alert.show" dismissible :variant="alert.type">
            {{ alert.message }}
        </BootstrapAlert>

        <CardElement :text="$t('pluginadmin.PluginList')" textVariant="text-bg-primary" add-space>
            <div class="table-responsive">
                <table class="table">
                    <thead>
                        <tr>
                            <th scope="col">{{ $t('pluginadmin.Name') }}</th>
                            <th>{{ $t('pluginadmin.Status') }}</th>
                            <th>{{ $t('pluginadmin.Action') }}</th>
                        </tr>
                    </thead>
                    <tbody ref="pList">
                        <tr v-for="plugin in plugins" v-bind:key="plugin['name']" :data-id="plugin['name']">
                            <td>{{ plugin['name'] }}</td>
                            <td>
                                {{ plugin['enabled'] }}
                            </td>
                            <td>
                                <a href="#" class="icon" :title="$t('pluginadmin.EditPlugin')">
                                    <BIconPencil v-on:click="onOpenModal(modal, plugin)" />
                                </a>
                            </td>
                        </tr>
                    </tbody>
                </table>
            </div>
        </CardElement>
    </BasePage>

    <div class="modal" id="pluginEdit" tabindex="-1">
        <div class="modal-dialog modal-lg">
            <div class="modal-content">
                <div class="modal-header">
                    <h5 class="modal-title">{{ $t('pluginadmin.EditPlugin') }}</h5>
                    <button type="button" class="btn-close" data-bs-dismiss="modal" aria-label="Close"></button>
                </div>
                <div class="modal-body">
                    <form>
                        <div class="mb-3">
                            <label for="plugin-id" class="col-form-label">
                                {{ $t('pluginadmin.Plugin') }}
                            </label>
                            <p v-for="(value, propertyName) in selectedPluginData" :key="propertyName">
                            <!-- {{ propertyName }}:{{ value }}:{{ idx }} -->
                            <label class="col-sm-2 col-form-label">{{ propertyName }}: </label>
                            <span v-if="typeof(value)==='number'" >
                                <input v-model="selectedPluginData[propertyName]" type="number"/>
                            </span>
                            <span v-else-if="typeof(value)==='boolean'">
                                <input v-model="selectedPluginData[propertyName]" type="checkbox"/>
                            </span> 
                            <span v-else-if="propertyName===pluginconfig">
                                <textarea v-model="selectedPluginConfig[selectedPluginData[pluginname]]"></textarea>
                            </span> 
                            <span v-else>
                                <input v-model="selectedPluginData[propertyName]" type="text" maxlength="384"/>
                            </span>
                            </p>
                        </div>
                    </form>

                </div>
                <div class="modal-footer">
                    <button type="button" class="btn btn-secondary" @click="onCloseModal(modal)"
                        data-bs-dismiss="modal">{{ $t('pluginadmin.Cancel') }}</button>
                    <button type="button" class="btn btn-primary" @click="onEditSubmit">
                        {{ $t('pluginadmin.Save') }}</button>
                </div>
            </div>
        </div>
    </div>

</template>

<script lang="ts">
import BasePage from '@/components/BasePage.vue';
import BootstrapAlert from "@/components/BootstrapAlert.vue";
import CardElement from '@/components/CardElement.vue';
import Sortable from 'sortablejs';
import { authHeader, handleResponse } from '@/utils/authentication';
import * as bootstrap from 'bootstrap';

import {
    BIconPencil
} from 'bootstrap-icons-vue';
import { defineComponent } from 'vue';

declare interface AlertResponse {
    message: string;
    type: string;
    code: number;
    show: boolean;
}

export default defineComponent({
    components: {
        BasePage,
        BootstrapAlert,
        CardElement,
        BIconPencil,
    },
    data() {
        return {
            modal: {} as bootstrap.Modal,
            newPluginData: {},
            selectedPluginData: {} as object,
            selectedPluginConfig: {},
            plugins: [],
            dataLoading: true,
            alert: {} as AlertResponse,
            sortable: {} as Sortable,
            el: {},
            pluginname: "name" as never,
            pluginconfig: "config" as never
        };
    },
    mounted() {
        this.modal = new bootstrap.Modal('#pluginEdit');
        this.el = document;
    },
    created() {
        this.getPlugins();
    },
    methods: {
        getPlugins() {
            this.dataLoading = true;
            fetch("/api/plugin/list", { headers: authHeader() })
                .then((response) => handleResponse(response, this.$emitter, this.$router))
                .then((data) => {
                    this.plugins = data.plugins;
                    this.dataLoading = false;

                    this.$nextTick(() => {
                        const table = this.$refs.pList as HTMLElement;

                        this.sortable = Sortable.create(table, {
                            sort: true,
                            handle: '.drag-handle',
                            animation: 150,
                            draggable: 'tr',
                        });
                    });
                });
        },
        callPluginApiEndpoint(endpoint: string, jsonData: string) {
            const formData = new FormData();
            formData.append("data", jsonData);

            fetch("/api/plugin/" + endpoint, {
                method: "POST",
                headers: authHeader(),
                body: formData,
            })
                .then((response) => handleResponse(response, this.$emitter, this.$router))
                .then((data) => {
                    this.getPlugins();
                    this.alert = data;
                    this.alert.message = this.$t('apiresponse.' + data.code, data.param);
                    this.alert.show = true;
                });
        },
        onEditSubmit() {
            this.configStr2Json((this.selectedPluginData as any)[this.pluginname],this.pluginconfig);
            this.callPluginApiEndpoint("edit", JSON.stringify(this.selectedPluginData));
            this.onCloseModal(this.modal);
        },
        onOpenModal(modal: bootstrap.Modal, pplugin: {}) {
            // deep copy object for editing/deleting
            this.selectedPluginData = JSON.parse(JSON.stringify(pplugin));
            if(this.selectedPluginData[this.pluginconfig]!==null) {
                (this.selectedPluginConfig as any)[this.selectedPluginData[this.pluginname]] = 
                JSON.stringify(this.selectedPluginData[this.pluginconfig],null,2);
            }
            modal.show();
        },
        onCloseModal(modal: bootstrap.Modal) {
            modal.hide();
        },
        onSaveOrder() {
            this.callPluginApiEndpoint("order", JSON.stringify({ order: this.sortable.toArray() }));
        },
        configStr2Json(pluginName: string, propertyName: string) {
            
            try { 
                var configD = (this.selectedPluginConfig as any)[pluginName];
                (this.selectedPluginData as any)[propertyName] = JSON.parse(configD);
                
            } catch(error) { /* */ }
        }
    },
});
</script>

<style>
.drag-handle {
    cursor: grab;
}
</style>